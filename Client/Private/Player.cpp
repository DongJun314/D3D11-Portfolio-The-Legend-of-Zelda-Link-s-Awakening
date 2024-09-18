#include "stdafx.h"
#include "Player.h"

// Client
#include "Weapon.h"
#include "Body_Player.h"

#include "Animation.h"

#include "PotDemon.h"
#include "Zombie.h"
#include "BlueMoriblin.h"
#include "StalfosOrange.h"
#include "Cucco.h"
#include "BomberGreen.h"
#include "ChainIronBall.h"

#include "Cell.h"
#include "Layer.h"
#include "Map_GameA.h"
#include "Camera_Free.h"
#include "Effect_Texture.h"
#include "Particle_Point.h"

CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLandObject(_pDevice, _pContext)
{
}

CPlayer::CPlayer(const CPlayer& _rhs)
	: CLandObject(_rhs)
{
}

CComponent* CPlayer::Get_PartObjectComponent(const wstring& _strPartObjTag, const wstring& _strComTag)
{
	auto iter = m_mapPlayerParts.find(_strPartObjTag);
	if (iter == m_mapPlayerParts.end())
		return nullptr;

	return iter->second->Get_Component(_strComTag);
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)_pArg;
	{
		pGameObjectDesc->fSpeedPerSec = 4.f;
		pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	// 초기 플레이어 위치 세팅
	/*{
		_matrix WorldMatrix = XMMatrixSet
		(0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-1.f, 0.0f, 0.0f, 0.0f,
			3.44f, 10.5f, 59.f, 1.0f);

		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}*/

	// zombie
	{	/*_matrix WorldMatrix = XMMatrixSet		
		   (1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.f,  0.0f, 1.0f, 0.0f,
			-29.3f, 9.f, 31.f, 1.0f);*/

		// Boss Monster
		_vector vecPos = { 1.58f, 10.5f, 59.7f, 1.0f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);
		m_pTransformCom->TurnTo(CTransform::TO_LEFT, 0.f);
	}

	m_pEffectSlash = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),2));
	if (nullptr == m_pEffectSlash)
		return E_FAIL;

	m_pEffectLanding = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), 3));
	if (nullptr == m_pEffectLanding)
	 	return E_FAIL;
	  
	m_pParticlePoint = dynamic_cast<CParticle_Point*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), 5));
	if (nullptr == m_pParticlePoint)
		return E_FAIL;

	Safe_AddRef(m_pParticlePoint);
	Safe_AddRef(m_pEffectSlash);
	Safe_AddRef(m_pEffectLanding);
	
	return S_OK;
}

void CPlayer::Priority_Tick(_float _fTimeDelta)
{
	for (auto& Pair : m_mapPlayerParts)
		(Pair.second)->Priority_Tick(_fTimeDelta);
}

void CPlayer::Tick(_float _fTimeDelta)
{
	if (!m_bMiniGameA && !m_bIsBossStage && !m_bFirstView) // 3인칭 메인맵
		MainGame_KeyInput(_fTimeDelta);

	if (m_bMiniGameA && !m_bIsBossStage && !m_bFirstView) // 3인칭 미니게임A
		MiniGameA_KeyInput(_fTimeDelta);

	if (m_bIsBossStage && !m_bMiniGameA && !m_bFirstView) // 3인칭 보스게임
		BossStage_KeyInput(_fTimeDelta);

	if (m_bFirstView && !m_bMiniGameA && !m_bIsBossStage) // 1인칭 메인맵
		FirstView_KeyInput(_fTimeDelta);

	if (m_bFirstView && m_bMiniGameA && !m_bIsBossStage) // 1인칭 미니게임A
		FirstView_MiniGameA_KeyInput(_fTimeDelta);

	if (m_bFirstView && m_bIsBossStage && !m_bMiniGameA) // 1인칭 보스게임
		FirstView_BossStage_KeyInput(_fTimeDelta);

	/*if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(_fTimeDelta);
	}

	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(_fTimeDelta);
	}

	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(_fTimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(_fTimeDelta);
	}*/
	
	
	Throw_FireBalls(_fTimeDelta);


	// 어떤 상태도 아닐 때
	if(m_iState == STATE_END)
	{
		m_iState = STATE_IDLE;

		m_bIsUpJumped    = false;
		m_bIsDownJumped  = false;
		m_bIsLeftJumped  = false;
		m_bIsRightJumped = false;

		m_fDmgQuake = 0.05f;
		m_fDmgBomb = 0.3f;
	}

	for (auto& Pair : m_mapPlayerParts)
		(Pair.second)->Tick(_fTimeDelta);
}

void CPlayer::Late_Tick(_float _fTimeDelta)
{
	for (auto& Pair : m_mapPlayerParts)
		(Pair.second)->Late_Tick(_fTimeDelta);

	SetUp_OnGames_Terrain();

	_vector w = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#endif
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	if(!m_bMiniGameA && !m_bIsBossStage)
		m_pNavigationCom->Render();
	else if(m_bMiniGameA && !m_bIsBossStage)
		m_pMiniGameA_NavigationCom->Render();
	else if(m_bIsBossStage && !m_bMiniGameA)
		m_pBossStage_NavigationCom->Render();
#endif

	return S_OK;
}

void CPlayer::MainGame_KeyInput(_float _fTimeDelta)
{
	if (m_iState == STATE_DMG_QUAKE)
	{
		//Delete_PartObject(TEXT("Part_PotDemon_BomberGreen_Clone1"));
		CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
		wstring wstrBomberCount = pPopDemon->Get_BomberGreenCloneCount();
		pPopDemon->Delete_PartObject(TEXT("Part_PotDemon_BomberGreen_Clone1"));

	}
	//m_pGameInstance->PlayBGMLoop("0_Title(No Intro Version).mp3", 1.f);
	// 무적일 상태
	if (m_bIsInvincible)
	{
		m_iState = STATE_IDLE;

		if (m_fInvincibleTimer < m_fInvincibleDuration)
		{
			m_fInvincibleTimer += _fTimeDelta;
		}
		else
		{
			m_bIsInvincible = false;
			m_fInvincibleTimer = 0.f;

			m_bIsUpJumped = false;
			m_bIsDownJumped = false;
			m_bIsLeftJumped = false;
			m_bIsRightJumped = false;

			m_fDmgQuake = 0.05f;
			m_fDmgBomb = 0.3f;
		}
	}

	// 공격 방어 키 관련 조작
	{
		// Shield
		if (m_pGameInstance->Get_DIKeyState(DIK_Z)
			&& m_iState != STATE_SLASH
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SHIELD;

			if (!m_bShieldSound)
			{
				m_pGameInstance->PlaySoundEx("shield.wav", SOUNDTYPE::SHIELD, 1.f);
				m_bShieldSound = true;
			}
			/*if (!m_bShieldSound)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::SLASH);
				m_pGameInstance->StopSound(SOUNDTYPE::SHIELD);
				m_pGameInstance->StopSound(SOUNDTYPE::GUARD);
				m_pGameInstance->StopSound(SOUNDTYPE::SWING);
				m_pGameInstance->StopSound(SOUNDTYPE::HIT);
				
				m_bShieldSound = true;
			}*/
		}
		else
		{
			m_bShieldSound = false;
		}

		

		// Slash
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_X)
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_DMG_QUAKE
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsTakingDamage)
			{
				m_fChargingGauge += _fTimeDelta;

				if (m_fChargingGauge <= 0.5f)
				{
					m_iState = STATE_SLASH;
					m_pGameInstance->PlaySoundEx("slash1.wav", SOUNDTYPE::SLASH, 1.f);

					/*if (!m_bSlashSound)
					{
						m_pGameInstance->StopSound(SOUNDTYPE::SLASH);
						m_pGameInstance->StopSound(SOUNDTYPE::SHIELD);
						m_pGameInstance->StopSound(SOUNDTYPE::GUARD);
						m_pGameInstance->StopSound(SOUNDTYPE::SWING);
						m_pGameInstance->StopSound(SOUNDTYPE::HIT);

						m_pGameInstance->PlaySoundEx("LSword_Swing2.wav", SOUNDTYPE::SLASH, 0.1f);
						m_bSlashSound = true;
					}*/
				}
				else
				{
					m_iState = STATE_SLASH_LP;
					// 파티클 차징
					if(m_fChargingGauge >= 1.f)
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						//m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_END);

						if (!m_bChargingParticle)
						{
							m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
							m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
							m_bChargingParticle = true;
						}

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_CHARGE);
					}
				}
			}
			if (m_pGameInstance->Key_Up('X'))
			{
				if (m_fChargingGauge > 1.f)
				{
					m_iState = STATE_NORMAL_SLASH_ED;
					
					// 이펙트
					m_bRotateOnce = false;
					m_pEffectSlash->Set_EffectOnce(true);

					m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 0.5f);
					m_pGameInstance->PlaySoundEx("slashED.wav", SOUNDTYPE::SWING, 0.5f);

					// 파티클 spread
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

						m_bChargingParticle = false;
					}
				}

				m_fChargingGauge = 0.f;
			}
 
			if (m_pEffectSlash->Get_EffectOnce())
			{
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(m_pEffectSlash->Get_Component(TEXT("Com_Transform")));
				_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vPlayerPos.m128_f32[1] += 0.5f;
				pEffectTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
				m_pEffectSlash->Animate_Texture(false);
			}
		}

		// Slash + Shield
		if (m_pGameInstance->Key_Pressing('Z')
			&& m_iState == STATE_SLASH_LP
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SLASH_SHIELD;
			//m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::ULT, 0.5f);
		}

		// Ult
		{
			if (m_pGameInstance->Key_Pressing('C')
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE
				&& !m_bIsTakingDamage)
			{
				m_iState = STATE_Lv25_SWRODGET_ST;
				m_bUltLandingSound = false;
				m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
			}
			//"5_Obj_TreasureBox_Appear"

			if (m_bUltJump)
			{
				m_fUltJumpSpeed -= 0.1f;

				m_bCancelOnTerrain = true;


				// 사전에 낙하 지점의 좌표를 구하기 위해, Look벡터를 구해줍니다
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.5f);
					m_pGameInstance->PlaySoundEx("Link_Warp_Ready.wav", SOUNDTYPE::SWING, 0.1f);

					m_vOriginalLook = { 0.f,0.f,0.f };
					XMStoreFloat3(&m_vOriginalLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
					
					m_bGetPosOnce = true;
				}

				m_pTransformCom->Go_Up(_fTimeDelta * m_fUltJumpSpeed);
			}
			// 사전에 낙하 지점의 좌표를 구하고, 그 좌표로 Look벡터를 변경해 줍니다
			if (m_bUltST)
			{
				m_fUltJumpSpeed = 3.5f;
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook) * 8.f;
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				{
					// 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
					vNewLandingPos = XMVector3TransformCoord(vNewLandingPos, m_pNavTransform->Get_WorldMatrix_Inverse());

					// 해당 객체의 Y축 좌표를 계산해서 반환한다 
					_int iCellIndex = { -1 };

					if (m_pNavigationCom->Is_Move(vNewLandingPos))
					{
						iCellIndex = m_pNavigationCom->Get_CellIndex();
						vNewLandingPos = XMVectorSetY(vNewLandingPos, m_vecCells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vNewLandingPos));
						m_fCellHeight = vNewLandingPos.m128_f32[1];

					}

					if (!m_bUltSound)
					{
						m_pGameInstance->PlaySoundEx("ultST.wav", SOUNDTYPE::ULT, 0.5f);
						m_bUltSound = true;
					}
				}

				m_pTransformCom->LookAt(vNewLandingPos);
			}
			if (m_bUltLP) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_pTransformCom->Go_Straight(_fTimeDelta * 5.f, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("ultLP.wav", SOUNDTYPE::LANDING, 0.5f);

				if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= (m_fCellHeight + 0.05f))
				{
					m_bUltLP = false;
					m_bUltED = true;
				}
			}
			if (m_bUltED
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_bUltSound = false;
				m_bGetPosOnce = false;
				m_iState = STATE_DASH_ED;
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook);
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				m_pTransformCom->LookAt(vNewLandingPos);
				

				m_bLockPos = false;
				m_vLockPos = { 0.f, 0.2f, 0.f, 0.f };
				m_pEffectLanding->Set_EffectOnce(true);

				if (!m_bUltLandingSound)
				{
					m_pGameInstance->PlaySoundEx("landing.wav", SOUNDTYPE::ULT, 1.f);
					m_bUltLandingSound = true;
				}
				
				// 파티클 spread
				{
					_float4x4 WorldMatrix;
					_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
					for (size_t i = 0; i < 3; i++)
					{
						BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
					}
					XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

					if (!m_bChargingParticle)
					{
						m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
						m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 1.f);
						m_bChargingParticle = true;
					}

					m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
					m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

					m_bChargingParticle = false;
				}
			}
			if (m_pEffectLanding->Get_EffectOnce())
			{
				if (!m_bLockPos)
				{
					m_vLockPos = m_vLockPos + m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bLockPos = true;
				}

				dynamic_cast<CTransform*>(m_pEffectLanding->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_vLockPos);
				m_pEffectLanding->Show_Texture();
			}
		}

	}

	// Only Walking
	if (m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_NORMAL_SLASH_ED
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정 방향키 관련 조작
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}

		// 대각선 방향 처리
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FLEFT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FRIGHT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BLEFT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BRIGHT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}

	// Shield + Walking
	if (m_iState == STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정방향
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}

		// 대각선 방향 처리
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_FLEFT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_FRIGHT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_BLEFT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_BRIGHT, _fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}

	// Slash + Walking
	if (m_iState == STATE_SLASH_LP
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		//_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		if (m_pGameInstance->Get_DIKeyState(DIK_UP))
		{
			m_iState = STATE_SLASH_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SLASH_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_SLASH_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SLASH_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// (Slash & Shield) + Walking
	if (m_iState == STATE_SLASH_SHIELD
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_UP))
		{
			m_iState = STATE_SLASH_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SLASH_SHIELD_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_SLASH_SHIELD_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SLASH_SHIELD_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// 점프
	{
		// 정 방향 + 대각선 방향
		{
			// 정방향
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsUpJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsUpJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}

			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsDownJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsDownJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}

			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsLeftJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsLeftJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}

			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsRightJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsRightJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
		}

		// 점프 + bool 값
		{
			// 정방향 bool 값
			if (m_bIsUpJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pNavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] + m_fJumpSpeed, vPos.m128_f32[3] };

					if (true == m_pNavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsUpJumped = false;
				}
			}
			if (m_bIsDownJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pNavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] - m_fJumpSpeed, vPos.m128_f32[3] };

					if (true == m_pNavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsDownJumped = false;
				}
			}
			if (m_bIsLeftJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pNavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0] - m_fJumpSpeed, vPos.m128_f32[1], vPos.m128_f32[2], vPos.m128_f32[3] };

					if (true == m_pNavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsLeftJumped = false;
				}
			}
			if (m_bIsRightJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pNavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0] + m_fJumpSpeed, vPos.m128_f32[1], vPos.m128_f32[2], vPos.m128_f32[3] };

					if (true == m_pNavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsRightJumped = false;
				}
			}
		}
	}

	// 링크가 데미지를 받고 있습니다
	if (m_iState == STATE_DMG_QUAKE && !m_bIsInvincible)
	{
		// 뒤로 물러가는 로직 
		{
			m_bIsTakingDamage = true;

			m_fDmgQuake -= 0.001f;
			m_fDmgBomb -= 0.005f;

			CZombie* pZombie;
			CBlueMoriblin* pBlueMoriblin;
			CStalfosOrange* pStalfosOrange;

			CCucco* pCucco;
			CBomberGreen* pBomberGreen;
			CChainIronBall* pChainIronBall;

			CCollider* pMonsterCollider = { nullptr };

			switch (m_iCollMonster)
			{
			case BLUEMORIBLIN:
				if (!m_bGetPosOnce)
				{
					pBlueMoriblin = dynamic_cast<CBlueMoriblin*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 0));
					pMonsterCollider = dynamic_cast<CCollider*>(pBlueMoriblin->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pBlueMoriblin->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case ZOMBIE:
				if (!m_bGetPosOnce)
				{
					pZombie = dynamic_cast<CZombie*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 1));
					pMonsterCollider = dynamic_cast<CCollider*>(pZombie->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pZombie->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case STALFOSORANGE:
				if (!m_bGetPosOnce)
				{
					pStalfosOrange = dynamic_cast<CStalfosOrange*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 2));
					//pMonsterCollider = dynamic_cast<CCollider*>(pStalfosOrange->Get_Component(TEXT("Com_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pStalfosOrange->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case POTDEMON:
			{
				CPotDemon* pPotDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				CTransform* pTransform = dynamic_cast<CTransform*>(pPotDemon->Get_Component(TEXT("Com_Transform")));
				pMonsterCollider = dynamic_cast<CCollider*>(pPotDemon->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pTransform->Get_WorldMatrix().r[3].m128_f32[0], pTransform->Get_WorldMatrix().r[3].m128_f32[1], pTransform->Get_WorldMatrix().r[3].m128_f32[2] };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case DEMONKING_L:
				break;

			case DEMONKING_R:
				break;

			case CUCCO:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
				pMonsterCollider = dynamic_cast<CCollider*>(pCucco->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}


			case BOMBERGREEN:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pBomberGreen = dynamic_cast<CBomberGreen*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_BomberGreen")));
				pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pBomberGreen->Get_PartsWorldMatrix()._41, pBomberGreen->Get_PartsWorldMatrix()._42, pBomberGreen->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case CHAINIRONBALL:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pChainIronBall = dynamic_cast<CChainIronBall*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_ChainIronBall")));
				pMonsterCollider = dynamic_cast<CCollider*>(pChainIronBall->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pChainIronBall->Get_PartsWorldMatrix()._41, pChainIronBall->Get_PartsWorldMatrix()._42, pChainIronBall->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case RANSENGAN:
				/*if (!m_bGetPosOnce)
				{
					CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
					CCucco* pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
					pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}*/
				break;
			}

			_vector vDirToPlayer = XMVector3Normalize(m_vPlayerPos - m_vMonsterPos);

			m_pTransformCom->LookAt_ForLandObject(m_vMonsterPos);


			if (m_fDmgQuake <= 0.f || m_fDmgBomb <= 0.f)
			{
				m_iState = STATE_IDLE;
				m_bGetPosOnce = false;
				m_bIsTakingDamage = false;
				m_bIsInvincible = true; // 데미지르 받고 직후 바로 무적인 상태로 만들기 위한 bool 값

				//m_fOverLap = 0.f;
				m_bGetOverlapOnce = false;
			}
			else // 몬스터 밀려나기
			{
				CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));

				if (m_iCollMonster == BOMBERGREEN
					|| m_iCollMonster == POTDEMON)
				{
					_vector vPlayerDir = vDirToPlayer * m_fDmgBomb;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pNavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);


				}
				else
				{
					_vector	vPlayerDir = vDirToPlayer * m_fDmgQuake;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pNavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);
				}
				pCamera->Shake_Camera(_fTimeDelta, 0.05f, 0.05f);

			}
		}
	}
}

void CPlayer::MiniGameA_KeyInput(_float _fTimeDelta)
{
	// 정 방향키 관련 조작
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// 대각선 방향 처리
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_FLEFT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_FRIGHT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_BLEFT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_WALK;
			m_pTransformCom->TurnTo(CTransform::TO_BRIGHT, _fTimeDelta, m_pMiniGameA_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}
}

void CPlayer::BossStage_KeyInput(_float _fTimeDelta)
{
	if (m_iState == STATE_DMG_QUAKE)
	{
		//Delete_PartObject(TEXT("Part_PotDemon_BomberGreen_Clone1"));
		CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
		wstring wstrBomberCount = pPopDemon->Get_BomberGreenCloneCount();
		pPopDemon->Delete_PartObject(TEXT("Part_PotDemon_BomberGreen_Clone1"));
	
	}
	
	// 무적일 상태
	if (m_bIsInvincible)
	{
		m_iState = STATE_IDLE;
	
		if (m_fInvincibleTimer < m_fInvincibleDuration)
		{
			m_fInvincibleTimer += _fTimeDelta;
		}
		else
		{
			m_bIsInvincible = false;
			m_fInvincibleTimer = 0.f;
	
			m_bIsUpJumped = false;
			m_bIsDownJumped = false;
			m_bIsLeftJumped = false;
			m_bIsRightJumped = false;
	
			m_fDmgQuake = 0.05f;
			m_fDmgBomb = 0.3f;
		}
	}
	
	// 공격 방어 키 관련 조작
	{
		// Shield
		if (m_pGameInstance->Get_DIKeyState(DIK_Z)
			&& m_iState != STATE_SLASH
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SHIELD;
				
			if(!m_bShieldSound)
			{
				m_pGameInstance->PlaySoundEx("shield.wav", SOUNDTYPE::SHIELD, 1.f);
				m_bShieldSound = true;
			}
			/*if (!m_bShieldSound)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::SLASH);
				m_pGameInstance->StopSound(SOUNDTYPE::SHIELD);
				m_pGameInstance->StopSound(SOUNDTYPE::GUARD);
				m_pGameInstance->StopSound(SOUNDTYPE::SWING);
				m_pGameInstance->StopSound(SOUNDTYPE::HIT);

				m_bShieldSound = true;
			}*/
		}
		else
		{
			m_bShieldSound = false;
		}
	
		// Slash
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_X)
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_DMG_QUAKE
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsTakingDamage)
			{
				m_fChargingGauge += _fTimeDelta;
	
				if (m_fChargingGauge <= 0.5f)
				{
					m_iState = STATE_SLASH;
					m_pGameInstance->PlaySoundEx("slash1.wav", SOUNDTYPE::SLASH, 1.f);
				}
				else
				{
					m_iState = STATE_SLASH_LP;
	
					// 파티클 차징
					if (m_fChargingGauge >= 1.f)
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());
	
						//m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_END);
	
						if (!m_bChargingParticle)
						{
							m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
							m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
							m_bChargingParticle = true;
						}
	
						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_CHARGE);
					}
				}
			}
			if (m_pGameInstance->Key_Up('X'))
			{
				if (m_fChargingGauge > 1.f)
				{
					m_iState = STATE_NORMAL_SLASH_ED;
	
					// 이펙트
					m_bRotateOnce = false;
					m_pEffectSlash->Set_EffectOnce(true);

					m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 0.5f);
					m_pGameInstance->PlaySoundEx("slashED.wav", SOUNDTYPE::SWING, 0.5f);
	
					// 파티클 spread
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());
	
						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);
	
						m_bChargingParticle = false;
					}
				}
	
				m_fChargingGauge = 0.f;
			}
	
			if (m_pEffectSlash->Get_EffectOnce())
			{
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(m_pEffectSlash->Get_Component(TEXT("Com_Transform")));
				_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vPlayerPos.m128_f32[1] += 0.5f;
				pEffectTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
				m_pEffectSlash->Animate_Texture(false);
			}
		}
	
		// Slash + Shield
		if (m_pGameInstance->Key_Pressing('Z')
			&& m_iState == STATE_SLASH_LP
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SLASH_SHIELD;
		}
	
		// Ult
		{
			if (m_pGameInstance->Key_Pressing('C')
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE
				&& !m_bIsTakingDamage)
			{
				m_iState = STATE_Lv25_SWRODGET_ST;
				m_bUltLandingSound = false;
				m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
			}
	
			if (m_bUltJump)
			{
				m_fUltJumpSpeed -= 0.1f;
	
				m_bCancelOnTerrain = true;
	
				// 사전에 낙하 지점의 좌표를 구하기 위해, Look벡터를 구해줍니다
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.5f);
					m_pGameInstance->PlaySoundEx("Link_Warp_Ready.wav", SOUNDTYPE::SWING, 0.1f);

					m_vOriginalLook = { 0.f,0.f,0.f };
					XMStoreFloat3(&m_vOriginalLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
					m_bGetPosOnce = true;
				}
	
				m_pTransformCom->Go_Up(_fTimeDelta * m_fUltJumpSpeed);
			}
			// 사전에 낙하 지점의 좌표를 구하고, 그 좌표로 Look벡터를 변경해 줍니다
			if (m_bUltST)
			{
				m_fUltJumpSpeed = 3.5f;
	
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook) * 8.f;
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				{
					// 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
					vNewLandingPos = XMVector3TransformCoord(vNewLandingPos, m_pBossStage_NavTransform->Get_WorldMatrix_Inverse());
	
					// 해당 객체의 Y축 좌표를 계산해서 반환한다 
					_int iCellIndex = { -1 };
	
					if (m_pBossStage_NavigationCom->Is_Move(vNewLandingPos))
					{
						iCellIndex = m_pBossStage_NavigationCom->Get_CellIndex();
						vNewLandingPos = XMVectorSetY(vNewLandingPos, m_vecBossStage_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vNewLandingPos));
						m_fCellHeight = vNewLandingPos.m128_f32[1];
					}

					if (!m_bUltSound)
					{
						m_pGameInstance->PlaySoundEx("ultST.wav", SOUNDTYPE::ULT, 0.5f);
						m_bUltSound = true;
					}
				}
	
				m_pTransformCom->LookAt(vNewLandingPos);
			}
			if (m_bUltLP) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_pTransformCom->Go_Straight(_fTimeDelta * 5.f, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("ultLP.wav", SOUNDTYPE::LANDING, 0.5f);
	
				if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= (m_fCellHeight + 0.05f))
				{
					m_bUltLP = false;
					m_bUltED = true;
				}
			}
			if (m_bUltED
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_bGetPosOnce = false;
				m_iState = STATE_DASH_ED;
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook);
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				m_pTransformCom->LookAt(vNewLandingPos);
	
				m_bLockPos = false;
				m_vLockPos = { 0.f, 0.2f, 0.f, 0.f };
				m_pEffectLanding->Set_EffectOnce(true);

				if (!m_bUltLandingSound)
				{
					m_pGameInstance->PlaySoundEx("landing.wav", SOUNDTYPE::ULT, 1.f);
					m_bUltLandingSound = true;
				}
	
				// 파티클 spread
				{
					_float4x4 WorldMatrix;
					_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
					for (size_t i = 0; i < 3; i++)
					{
						BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
					}
					XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());
	
					if (!m_bChargingParticle)
					{
						m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
						m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 1.f);
						m_bChargingParticle = true;
					}
	
					m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
					m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);
	
					m_bChargingParticle = false;
				}
			}
			if (m_pEffectLanding->Get_EffectOnce())
			{
				if (!m_bLockPos)
				{
					m_vLockPos = m_vLockPos + m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bLockPos = true;
				}
	
				dynamic_cast<CTransform*>(m_pEffectLanding->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_vLockPos);
				m_pEffectLanding->Show_Texture();
			}
		}
	
	}
	
	// Only Walking
	if (m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_NORMAL_SLASH_ED
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정 방향키 관련 조작
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	
		// 대각선 방향 처리
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FLEFT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_FRIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BLEFT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->TurnTo(CTransform::TO_BRIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}
	
	// Shield + Walking
	if (m_iState == STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정방향
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && !m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) && !m_pGameInstance->Get_DIKeyState(DIK_UP) && !m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	
		// 대각선 방향 처리
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_FLEFT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_FRIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_LEFT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_BLEFT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) && m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
			{
				m_iState = STATE_SHIELD_F;
				m_pTransformCom->TurnTo(CTransform::TO_BRIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}
	
	// Slash + Walking
	if (m_iState == STATE_SLASH_LP
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		//_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	
		if (m_pGameInstance->Get_DIKeyState(DIK_UP))
		{
			m_iState = STATE_SLASH_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SLASH_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_SLASH_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SLASH_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}
	
	// (Slash & Shield) + Walking
	if (m_iState == STATE_SLASH_SHIELD
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_UP))
		{
			m_iState = STATE_SLASH_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN))
		{
			m_iState = STATE_SLASH_SHIELD_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT))
		{
			m_iState = STATE_SLASH_SHIELD_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT))
		{
			m_iState = STATE_SLASH_SHIELD_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}
	
	// 점프
	{
		// 정 방향 + 대각선 방향
		{
			// 정방향
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsUpJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsUpJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
	
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsDownJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsDownJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
	
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsLeftJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsLeftJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
	
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_RIGHT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_LEFT)
				&& !m_pGameInstance->Get_DIKeyState(DIK_UP)
				&& !m_pGameInstance->Get_DIKeyState(DIK_DOWN)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsRightJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsRightJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
		}
	
		// 점프 + bool 값
		{
			// 정방향 bool 값
			if (m_bIsUpJumped)
			{
				m_fJumpTime += _fTimeDelta;
	
				m_pTransformCom->TurnTo(CTransform::TO_FORWARD, _fTimeDelta, m_pBossStage_NavigationCom);
	
				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] + m_fJumpSpeed, vPos.m128_f32[3] };
	
					if (true == m_pBossStage_NavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsUpJumped = false;
				}
			}
			if (m_bIsDownJumped)
			{
				m_fJumpTime += _fTimeDelta;
	
				m_pTransformCom->TurnTo(CTransform::TO_BACKWARD, _fTimeDelta, m_pBossStage_NavigationCom);
	
				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2] - m_fJumpSpeed, vPos.m128_f32[3] };
	
					if (true == m_pBossStage_NavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsDownJumped = false;
				}
			}
			if (m_bIsLeftJumped)
			{
				m_fJumpTime += _fTimeDelta;
	
				m_pTransformCom->TurnTo(CTransform::TO_LEFT, _fTimeDelta, m_pBossStage_NavigationCom);
	
				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0] - m_fJumpSpeed, vPos.m128_f32[1], vPos.m128_f32[2], vPos.m128_f32[3] };
	
					if (true == m_pBossStage_NavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsLeftJumped = false;
				}
			}
			if (m_bIsRightJumped)
			{
				m_fJumpTime += _fTimeDelta;
	
				m_pTransformCom->TurnTo(CTransform::TO_RIGHT, _fTimeDelta, m_pBossStage_NavigationCom);
	
				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_fvector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_fvector vNewPos = { vPos.m128_f32[0] + m_fJumpSpeed, vPos.m128_f32[1], vPos.m128_f32[2], vPos.m128_f32[3] };
	
					if (true == m_pBossStage_NavigationCom->Is_Move(vNewPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsRightJumped = false;
				}
			}
		}
	}
	
	// 링크가 데미지를 받고 있습니다
	if (m_iState == STATE_DMG_QUAKE && !m_bIsInvincible)
	{
		// 뒤로 물러가는 로직 
		{
			m_bIsTakingDamage = true;
	
			m_fDmgQuake -= 0.001f;
			m_fDmgBomb -= 0.005f;
	
			CZombie* pZombie;
			CBlueMoriblin* pBlueMoriblin;
			CStalfosOrange* pStalfosOrange;
	
			CCucco* pCucco;
			CBomberGreen* pBomberGreen;
			CChainIronBall* pChainIronBall;
	
			CCollider* pMonsterCollider = { nullptr };
	
			switch (m_iCollMonster)
			{
			case BLUEMORIBLIN:
				if (!m_bGetPosOnce)
				{
					pBlueMoriblin = dynamic_cast<CBlueMoriblin*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 0));
					pMonsterCollider = dynamic_cast<CCollider*>(pBlueMoriblin->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pBlueMoriblin->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
	
			case ZOMBIE:
				if (!m_bGetPosOnce)
				{
					pZombie = dynamic_cast<CZombie*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 1));
					pMonsterCollider = dynamic_cast<CCollider*>(pZombie->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pZombie->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
	
			case STALFOSORANGE:
				if (!m_bGetPosOnce)
				{
					pStalfosOrange = dynamic_cast<CStalfosOrange*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 2));
					//pMonsterCollider = dynamic_cast<CCollider*>(pStalfosOrange->Get_Component(TEXT("Com_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pStalfosOrange->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
	
			case POTDEMON:
			{
				CPotDemon* pPotDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				CTransform* pTransform = dynamic_cast<CTransform*>(pPotDemon->Get_Component(TEXT("Com_Transform")));
				pMonsterCollider = dynamic_cast<CCollider*>(pPotDemon->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pTransform->Get_WorldMatrix().r[3].m128_f32[0], pTransform->Get_WorldMatrix().r[3].m128_f32[1], pTransform->Get_WorldMatrix().r[3].m128_f32[2] };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}
	
			case DEMONKING_L:
				break;
	
			case DEMONKING_R:
				break;
	
			case CUCCO:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
				pMonsterCollider = dynamic_cast<CCollider*>(pCucco->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}
	
	
			case BOMBERGREEN:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pBomberGreen = dynamic_cast<CBomberGreen*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_BomberGreen")));
				pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pBomberGreen->Get_PartsWorldMatrix()._41, pBomberGreen->Get_PartsWorldMatrix()._42, pBomberGreen->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}
	
			case CHAINIRONBALL:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pChainIronBall = dynamic_cast<CChainIronBall*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_ChainIronBall")));
				pMonsterCollider = dynamic_cast<CCollider*>(pChainIronBall->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pChainIronBall->Get_PartsWorldMatrix()._41, pChainIronBall->Get_PartsWorldMatrix()._42, pChainIronBall->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}
	
			case RANSENGAN:
				/*if (!m_bGetPosOnce)
				{
					CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
					CCucco* pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
					pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}*/
				break;
			}
	
			_vector vDirToPlayer = XMVector3Normalize(m_vPlayerPos - m_vMonsterPos);
	
			m_pTransformCom->LookAt_ForLandObject(m_vMonsterPos);
	
	
			if (m_fDmgQuake <= 0.f || m_fDmgBomb <= 0.f)
			{
				m_iState = STATE_IDLE;
				m_bGetPosOnce = false;
				m_bIsTakingDamage = false;
				m_bIsInvincible = true; // 데미지르 받고 직후 바로 무적인 상태로 만들기 위한 bool 값
	
				//m_fOverLap = 0.f;
				m_bGetOverlapOnce = false;
			}
			else // 몬스터 밀려나기
			{
				CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));
	
				if (m_iCollMonster == BOMBERGREEN
					|| m_iCollMonster == POTDEMON)
				{
					_vector vPlayerDir = vDirToPlayer * m_fDmgBomb;
	
					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);
	
					if (true == m_pBossStage_NavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);
	
	
				}
				else
				{
					_vector	vPlayerDir = vDirToPlayer * m_fDmgQuake;
	
					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);
	
					if (true == m_pBossStage_NavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);
				}
				pCamera->Shake_Camera(_fTimeDelta, 0.05f, 0.05f);
	
			}
		}
	}
}

void CPlayer::FirstView_KeyInput(_float _fTimeDelta)
{
	// 무적일 상태
	if (m_bIsInvincible)
	{
		m_iState = STATE_IDLE;

		if (m_fInvincibleTimer < m_fInvincibleDuration)
		{
			m_fInvincibleTimer += _fTimeDelta;
		}
		else
		{
			m_bIsInvincible = false;
			m_fInvincibleTimer = 0.f;

			m_bIsUpJumped = false;
			m_bIsDownJumped = false;
			m_bIsLeftJumped = false;
			m_bIsRightJumped = false;

			m_fDmgQuake = 0.05f;
			m_fDmgBomb = 0.3f;
		}
	}

	// 공격 방어 키 관련 조작
	{
		// Shield
		if (m_pGameInstance->Get_DIMouseState(DIM_RB)
			&& m_iState != STATE_SLASH
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SHIELD;
			if (!m_bShieldSound)
			{
				m_pGameInstance->PlaySoundEx("shield.wav", SOUNDTYPE::SHIELD, 1.f);
				m_bShieldSound = true;
			}
			/*if (!m_bShieldSound)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::SLASH);
				m_pGameInstance->StopSound(SOUNDTYPE::SHIELD);
				m_pGameInstance->StopSound(SOUNDTYPE::GUARD);
				m_pGameInstance->StopSound(SOUNDTYPE::SWING);
				m_pGameInstance->StopSound(SOUNDTYPE::HIT);

				m_bShieldSound = true;
			}*/
		}
		else
		{
			m_bShieldSound = false;
		}

		// Slash
		{
			if (m_pGameInstance->Get_DIMouseState(DIM_LB)
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_DMG_QUAKE
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsTakingDamage)
			{
				m_fChargingGauge += _fTimeDelta;

				if (m_fChargingGauge <= 0.5f)
				{
					m_iState = STATE_SLASH;
					m_pGameInstance->PlaySoundEx("slash1.wav", SOUNDTYPE::SLASH, 1.f);
				}
				else
				{
					m_iState = STATE_SLASH_LP;


					// 파티클 차징
					if (m_fChargingGauge >= 1.f)
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						//m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_END);

						if (!m_bChargingParticle)
						{
							m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
							m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
							m_bChargingParticle = true;
						}

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_CHARGE);
					}
				}
			}
			if (m_pGameInstance->Key_Up(MK_LBUTTON))
			{
				if (m_fChargingGauge > 1.f)
				{
					m_iState = STATE_NORMAL_SLASH_ED;

					// 이펙트
					m_bRotateOnce = false;
					m_pEffectSlash->Set_EffectOnce(true);

					m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 0.5f);
					m_pGameInstance->PlaySoundEx("slashED.wav", SOUNDTYPE::SWING, 0.5f);

					// 파티클 spread
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

						m_bChargingParticle = false;
					}
				}

				m_fChargingGauge = 0.f;
			}

			if (m_pEffectSlash->Get_EffectOnce())
			{
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(m_pEffectSlash->Get_Component(TEXT("Com_Transform")));
				_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vPlayerPos.m128_f32[1] += 0.5f;
				pEffectTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
				m_pEffectSlash->Animate_Texture(false);
			}
		}


		// Slash + Shield
		if (m_pGameInstance->Get_DIMouseState(DIM_RB)
			&& m_pGameInstance->Get_DIMouseState(DIM_LB)
			&& m_iState == STATE_SLASH_LP
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SLASH_SHIELD;
		}
		// Ult
		{
			if (m_pGameInstance->Get_DIMouseState(DIM_MB)
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE
				&& !m_bIsTakingDamage)
			{
				m_iState = STATE_Lv25_SWRODGET_ST;
				m_bUltLandingSound = false;
				m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
			}

			if (m_bUltJump)
			{
				m_fUltJumpSpeed -= 0.1f;

				m_bCancelOnTerrain = true;

				// 사전에 낙하 지점의 좌표를 구하기 위해, Look벡터를 구해줍니다
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.5f);
					m_pGameInstance->PlaySoundEx("Link_Warp_Ready.wav", SOUNDTYPE::SWING, 0.1f);

					m_vOriginalLook = { 0.f,0.f,0.f };
					XMStoreFloat3(&m_vOriginalLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
					m_bGetPosOnce = true;
				}

				m_pTransformCom->Go_Up(_fTimeDelta * m_fUltJumpSpeed);
			}
			// 사전에 낙하 지점의 좌표를 구하고, 그 좌표로 Look벡터를 변경해 줍니다
			if (m_bUltST)
			{
				m_fUltJumpSpeed = 3.5f;

				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook) * 8.f;
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				{
					// 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
					vNewLandingPos = XMVector3TransformCoord(vNewLandingPos, m_pNavTransform->Get_WorldMatrix_Inverse());

					// 해당 객체의 Y축 좌표를 계산해서 반환한다 
					_int iCellIndex = { -1 };

					if (m_pNavigationCom->Is_Move(vNewLandingPos))
					{
						iCellIndex = m_pNavigationCom->Get_CellIndex();
						vNewLandingPos = XMVectorSetY(vNewLandingPos, m_vecCells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vNewLandingPos));
						m_fCellHeight = vNewLandingPos.m128_f32[1];
					}

					if (!m_bUltSound)
					{
						m_pGameInstance->PlaySoundEx("ultST.wav", SOUNDTYPE::ULT, 0.5f);
						m_bUltSound = true;
					}
				}

				m_pTransformCom->LookAt(vNewLandingPos);
			}
			if (m_bUltLP) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_pTransformCom->Go_Straight(_fTimeDelta * 5.f, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("ultLP.wav", SOUNDTYPE::LANDING, 0.5f);

				if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= (m_fCellHeight + 0.05f))
				{
					m_bUltLP = false;
					m_bUltED = true;
				}
			}
			if (m_bUltED
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_bGetPosOnce = false;
				m_iState = STATE_DASH_ED;
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook);
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				m_pTransformCom->LookAt(vNewLandingPos);

				m_bLockPos = false;
				m_vLockPos = { 0.f, 0.2f, 0.f, 0.f };
				m_pEffectLanding->Set_EffectOnce(true);

				if (!m_bUltLandingSound)
				{
					m_pGameInstance->PlaySoundEx("landing.wav", SOUNDTYPE::ULT, 1.f);
					m_bUltLandingSound = true;
				}

				// 파티클 spread
				{
					_float4x4 WorldMatrix;
					_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
					for (size_t i = 0; i < 3; i++)
					{
						BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
					}
					XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

					if (!m_bChargingParticle)
					{
						m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
						m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 1.f);
						m_bChargingParticle = true;
					}

					m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
					m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

					m_bChargingParticle = false;
				}
			}
			if (m_pEffectLanding->Get_EffectOnce())
			{
				if (!m_bLockPos)
				{
					m_vLockPos = m_vLockPos + m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bLockPos = true;
				}

				dynamic_cast<CTransform*>(m_pEffectLanding->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_vLockPos);
				m_pEffectLanding->Show_Texture();
			}
		}
	}

	// Only Walking
	if (m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_NORMAL_SLASH_ED
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정 방향키 관련 조작
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_S) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_D) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}

	// Shield + Walking
	if (m_iState == STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정방향
		if (m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// Slash + Walking
	if (m_iState == STATE_SLASH_LP
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		//_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		if (m_pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_iState = STATE_SLASH_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SLASH_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_iState = STATE_SLASH_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SLASH_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// (Slash & Shield) + Walking
	if (m_iState == STATE_SLASH_SHIELD
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_iState = STATE_SLASH_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SLASH_SHIELD_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_iState = STATE_SLASH_SHIELD_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SLASH_SHIELD_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// 점프
	{
		// 정 방향 + 대각선 방향
		{
			// 정방향
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_W)
				&& !m_pGameInstance->Get_DIKeyState(DIK_S)
				&& !m_pGameInstance->Get_DIKeyState(DIK_A)
				&& !m_pGameInstance->Get_DIKeyState(DIK_D)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsUpJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsUpJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
		}

		// 점프 + bool 값
		{
			// 정방향 bool 값
			if (m_bIsUpJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

					vPos += XMVector3Normalize(vLook) * m_fJumpSpeed * 60.f * _fTimeDelta;

					if (true == m_pNavigationCom->Is_Move(vPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsUpJumped = false;
				}
			}
		}
	}

	// 링크가 데미지를 받고 있습니다
	if (m_iState == STATE_DMG_QUAKE && !m_bIsInvincible)
	{
		// 뒤로 물러가는 로직 
		{
			m_bIsTakingDamage = true;

			m_fDmgQuake -= 0.001f;
			m_fDmgBomb -= 0.005f;

			CZombie* pZombie;
			CBlueMoriblin* pBlueMoriblin;
			CStalfosOrange* pStalfosOrange;

			CCucco* pCucco;
			CBomberGreen* pBomberGreen;
			CChainIronBall* pChainIronBall;

			CCollider* pMonsterCollider = { nullptr };

			switch (m_iCollMonster)
			{
			case BLUEMORIBLIN:
				if (!m_bGetPosOnce)
				{
					pBlueMoriblin = dynamic_cast<CBlueMoriblin*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 0));
					pMonsterCollider = dynamic_cast<CCollider*>(pBlueMoriblin->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pBlueMoriblin->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case ZOMBIE:
				if (!m_bGetPosOnce)
				{
					pZombie = dynamic_cast<CZombie*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 1));
					pMonsterCollider = dynamic_cast<CCollider*>(pZombie->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pZombie->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case STALFOSORANGE:
				if (!m_bGetPosOnce)
				{
					pStalfosOrange = dynamic_cast<CStalfosOrange*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 2));
					//pMonsterCollider = dynamic_cast<CCollider*>(pStalfosOrange->Get_Component(TEXT("Com_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pStalfosOrange->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case POTDEMON:
			{
				CPotDemon* pPotDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				CTransform* pTransform = dynamic_cast<CTransform*>(pPotDemon->Get_Component(TEXT("Com_Transform")));
				pMonsterCollider = dynamic_cast<CCollider*>(pPotDemon->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pTransform->Get_WorldMatrix().r[3].m128_f32[0], pTransform->Get_WorldMatrix().r[3].m128_f32[1], pTransform->Get_WorldMatrix().r[3].m128_f32[2] };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case DEMONKING_L:
				break;

			case DEMONKING_R:
				break;

			case CUCCO:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
				pMonsterCollider = dynamic_cast<CCollider*>(pCucco->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}


			case BOMBERGREEN:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pBomberGreen = dynamic_cast<CBomberGreen*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_BomberGreen")));
				pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pBomberGreen->Get_PartsWorldMatrix()._41, pBomberGreen->Get_PartsWorldMatrix()._42, pBomberGreen->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case CHAINIRONBALL:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pChainIronBall = dynamic_cast<CChainIronBall*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_ChainIronBall")));
				pMonsterCollider = dynamic_cast<CCollider*>(pChainIronBall->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pChainIronBall->Get_PartsWorldMatrix()._41, pChainIronBall->Get_PartsWorldMatrix()._42, pChainIronBall->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case RANSENGAN:
				/*if (!m_bGetPosOnce)
				{
					CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
					CCucco* pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
					pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}*/
				break;
			}

			_vector vDirToPlayer = XMVector3Normalize(m_vPlayerPos - m_vMonsterPos);

			//m_pTransformCom->LookAt_ForLandObject(m_vMonsterPos);


			if (m_fDmgQuake <= 0.f || m_fDmgBomb <= 0.f)
			{
				m_iState = STATE_IDLE;
				m_bGetPosOnce = false;
				m_bIsTakingDamage = false;
				m_bIsInvincible = true; // 데미지르 받고 직후 바로 무적인 상태로 만들기 위한 bool 값

				//m_fOverLap = 0.f;
				m_bGetOverlapOnce = false;
			}
			else // 몬스터 밀려나기
			{
				CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));

				if (m_iCollMonster == BOMBERGREEN
					|| m_iCollMonster == POTDEMON)
				{
					_vector vPlayerDir = vDirToPlayer * m_fDmgBomb;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pNavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);


				}
				else
				{
					_vector	vPlayerDir = vDirToPlayer * m_fDmgQuake;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pNavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);
				}
				pCamera->Shake_Camera(_fTimeDelta, 0.05f, 0.05f);

			}
		}
	}
}

void CPlayer::FirstView_MiniGameA_KeyInput(_float _fTimeDelta)
{
	// Only Walking
	if (m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_NORMAL_SLASH_ED
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정 방향키 관련 조작
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Straight(_fTimeDelta, m_pMiniGameA_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_S) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Backward(_fTimeDelta, m_pMiniGameA_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Left(_fTimeDelta, m_pMiniGameA_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_D) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Right(_fTimeDelta, m_pMiniGameA_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}
}

void CPlayer::FirstView_BossStage_KeyInput(_float _fTimeDelta)
{
	// 무적일 상태
	if (m_bIsInvincible)
	{
		m_iState = STATE_IDLE;

		if (m_fInvincibleTimer < m_fInvincibleDuration)
		{
			m_fInvincibleTimer += _fTimeDelta;
		}
		else
		{
			m_bIsInvincible = false;
			m_fInvincibleTimer = 0.f;

			m_bIsUpJumped = false;
			m_bIsDownJumped = false;
			m_bIsLeftJumped = false;
			m_bIsRightJumped = false;

			m_fDmgQuake = 0.05f;
			m_fDmgBomb = 0.3f;
		}
	}

	// 공격 방어 키 관련 조작
	{
		// Shield
		if (m_pGameInstance->Get_DIMouseState(DIM_RB)
			&& m_iState != STATE_SLASH
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SHIELD;

			if (!m_bShieldSound)
			{
				m_pGameInstance->PlaySoundEx("shield.wav", SOUNDTYPE::SHIELD, 1.f);
				m_bShieldSound = true;
			}
			/*if (!m_bShieldSound)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::SLASH);
				m_pGameInstance->StopSound(SOUNDTYPE::SHIELD);
				m_pGameInstance->StopSound(SOUNDTYPE::GUARD);
				m_pGameInstance->StopSound(SOUNDTYPE::SWING);
				m_pGameInstance->StopSound(SOUNDTYPE::HIT);

				m_bShieldSound = true;
			}*/
		}
		else
		{
			m_bShieldSound = false;
		}

		// Slash
		{
			if (m_pGameInstance->Get_DIMouseState(DIM_LB)
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_DMG_QUAKE
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsTakingDamage)
			{
				m_fChargingGauge += _fTimeDelta;

				if (m_fChargingGauge <= 0.5f)
				{
					m_iState = STATE_SLASH;
					m_pGameInstance->PlaySoundEx("slash1.wav", SOUNDTYPE::SLASH, 1.f);
				}
				else
				{
					m_iState = STATE_SLASH_LP;


					// 파티클 차징
					if (m_fChargingGauge >= 1.f)
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						//m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_END);

						if (!m_bChargingParticle)
						{
							m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
							m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
							m_bChargingParticle = true;
						}

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_CHARGE);
					}
				}
			}
			if (m_pGameInstance->Key_Up(MK_LBUTTON))
			{
				if (m_fChargingGauge > 1.f)
				{
					m_iState = STATE_NORMAL_SLASH_ED;

					// 이펙트
					m_bRotateOnce = false;
					m_pEffectSlash->Set_EffectOnce(true);

					m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 0.5f);
					m_pGameInstance->PlaySoundEx("slashED.wav", SOUNDTYPE::SWING, 0.5f);

					// 파티클 spread
					{
						_float4x4 WorldMatrix;
						_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
						for (size_t i = 0; i < 3; i++)
						{
							BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
						}
						XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

						m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
						m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

						m_bChargingParticle = false;
					}
				}

				m_fChargingGauge = 0.f;
			}

			if (m_pEffectSlash->Get_EffectOnce())
			{
				CTransform* pEffectTransform = dynamic_cast<CTransform*>(m_pEffectSlash->Get_Component(TEXT("Com_Transform")));
				_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				vPlayerPos.m128_f32[1] += 0.5f;
				pEffectTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
				m_pEffectSlash->Animate_Texture(false);
			}
		}


		// Slash + Shield
		if (m_pGameInstance->Get_DIMouseState(DIM_RB)
			&& m_pGameInstance->Get_DIMouseState(DIM_LB)
			&& m_iState == STATE_SLASH_LP
			&& m_iState != STATE_NORMAL_SLASH_ED
			&& m_iState != STATE_DMG_QUAKE
			&& m_iState != STATE_Lv25_SWRODGET_ST
			&& !m_bUltJump
			&& !m_bUltST
			&& !m_bUltLP
			&& !m_bUltED
			&& !m_bIsTakingDamage)
		{
			m_iState = STATE_SLASH_SHIELD;
		}
		// Ult
		{
			if (m_pGameInstance->Get_DIMouseState(DIM_MB)
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE
				&& !m_bIsTakingDamage)
			{
				m_iState = STATE_Lv25_SWRODGET_ST;
				m_bUltLandingSound = false;
				m_pGameInstance->PlaySoundEx("charge.wav", SOUNDTYPE::CHARGE, 0.5f);
			}

			if (m_bUltJump)
			{
				m_fUltJumpSpeed -= 0.1f;

				m_bCancelOnTerrain = true;

				// 사전에 낙하 지점의 좌표를 구하기 위해, Look벡터를 구해줍니다
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.5f);
					m_pGameInstance->PlaySoundEx("Link_Warp_Ready.wav", SOUNDTYPE::SWING, 0.1f);

					m_vOriginalLook = { 0.f,0.f,0.f };
					XMStoreFloat3(&m_vOriginalLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
					m_bGetPosOnce = true;
				}

				m_pTransformCom->Go_Up(_fTimeDelta * m_fUltJumpSpeed);
			}
			// 사전에 낙하 지점의 좌표를 구하고, 그 좌표로 Look벡터를 변경해 줍니다
			if (m_bUltST)
			{
				m_fUltJumpSpeed = 3.5f;

				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook) * 8.f;
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				{
					// 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
					vNewLandingPos = XMVector3TransformCoord(vNewLandingPos, m_pBossStage_NavTransform->Get_WorldMatrix_Inverse());

					// 해당 객체의 Y축 좌표를 계산해서 반환한다 
					_int iCellIndex = { -1 };

					if (m_pBossStage_NavigationCom->Is_Move(vNewLandingPos))
					{
						iCellIndex = m_pBossStage_NavigationCom->Get_CellIndex();
						vNewLandingPos = XMVectorSetY(vNewLandingPos, m_vecBossStage_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vNewLandingPos));
						m_fCellHeight = vNewLandingPos.m128_f32[1];
					}

					if (!m_bUltSound)
					{
						m_pGameInstance->PlaySoundEx("ultST.wav", SOUNDTYPE::ULT, 0.5f);
						m_bUltSound = true;
					}
				}

				m_pTransformCom->LookAt(vNewLandingPos);
			}
			if (m_bUltLP) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_pTransformCom->Go_Straight(_fTimeDelta * 5.f, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("ultLP.wav", SOUNDTYPE::LANDING, 0.5f);

				if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= (m_fCellHeight + 0.05f))
				{
					m_bUltLP = false;
					m_bUltED = true;
				}
			}
			if (m_bUltED
				&& m_iState != STATE_SHIELD
				&& m_iState != STATE_SLASH_LP
				&& m_iState != STATE_SLASH_SHIELD
				&& m_iState != STATE_NORMAL_SLASH_ED
				&& m_iState != STATE_JUMP
				&& m_iState != STATE_DMG_QUAKE) // 낙하 지점의 좌표로 트랜스폼을 움직입니다
			{
				m_bGetPosOnce = false;
				m_iState = STATE_DASH_ED;
				_vector vNewLook = XMLoadFloat3(&m_vOriginalLook);
				_vector vNewLandingPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vNewLook;
				m_pTransformCom->LookAt(vNewLandingPos);

				m_bLockPos = false;
				m_vLockPos = { 0.f, 0.2f, 0.f, 0.f };
				m_pEffectLanding->Set_EffectOnce(true);

				if (!m_bUltLandingSound)
				{
					m_pGameInstance->PlaySoundEx("landing.wav", SOUNDTYPE::ULT, 1.f);
					m_bUltLandingSound = true;
				}

				// 파티클 spread
				{
					_float4x4 WorldMatrix;
					_matrix BoneMatrix = XMLoadFloat4x4(m_pParticle_SocketMatrix);
					for (size_t i = 0; i < 3; i++)
					{
						BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
					}
					XMStoreFloat4x4(&WorldMatrix, BoneMatrix * m_pTransformCom->Get_WorldMatrix());

					if (!m_bChargingParticle)
					{
						m_pParticlePoint->Reset_Particle_LifeTime(_fTimeDelta);
						m_pGameInstance->PlaySoundEx("particle.wav", SOUNDTYPE::PARTICLE, 1.f);
						m_bChargingParticle = true;
					}

					m_pParticlePoint->Set_ParticlePos(XMVectorSet(WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44));
					m_pParticlePoint->Set_ParticleType(CParticle_Point::PT_SPREAD);

					m_bChargingParticle = false;
				}
			}
			if (m_pEffectLanding->Get_EffectOnce())
			{
				if (!m_bLockPos)
				{
					m_vLockPos = m_vLockPos + m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bLockPos = true;
				}

				dynamic_cast<CTransform*>(m_pEffectLanding->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, m_vLockPos);
				m_pEffectLanding->Show_Texture();
			}
		}
	}

	// Only Walking
	if (m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_NORMAL_SLASH_ED
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정 방향키 관련 조작
		{
			if (m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_S) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_D) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_iState = STATE_WALK;
				m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
				m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
			}
		}
	}

	// Shield + Walking
	if (m_iState == STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		// 정방향
		if (m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S) && !m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D) && !m_pGameInstance->Get_DIKeyState(DIK_W) && !m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SHIELD_F;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// Slash + Walking
	if (m_iState == STATE_SLASH_LP
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_SHIELD
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		//_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

		if (m_pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_iState = STATE_SLASH_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SLASH_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_iState = STATE_SLASH_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SLASH_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// (Slash & Shield) + Walking
	if (m_iState == STATE_SLASH_SHIELD
		&& m_iState != STATE_SHIELD
		&& m_iState != STATE_SLASH_LP
		&& m_iState != STATE_JUMP
		&& m_iState != STATE_DMG_QUAKE
		&& m_iState != STATE_Lv25_SWRODGET_ST
		&& !m_bUltJump
		&& !m_bUltST
		&& !m_bUltLP
		&& !m_bUltED
		&& !m_bIsTakingDamage)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_iState = STATE_SLASH_SHIELD_F;
			m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_iState = STATE_SLASH_SHIELD_B;
			m_pTransformCom->Go_Backward(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_iState = STATE_SLASH_SHIELD_L;
			m_pTransformCom->Go_Left(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_iState = STATE_SLASH_SHIELD_R;
			m_pTransformCom->Go_Right(_fTimeDelta, m_pBossStage_NavigationCom);
			m_pGameInstance->PlaySoundEx("walk.wav", SOUNDTYPE::WALK, 0.3f);
		}
	}

	// 점프
	{
		// 정 방향 + 대각선 방향
		{
			// 정방향
			if (m_pGameInstance->Get_DIKeyState(DIK_SPACE)
				&& m_pGameInstance->Get_DIKeyState(DIK_W)
				&& !m_pGameInstance->Get_DIKeyState(DIK_S)
				&& !m_pGameInstance->Get_DIKeyState(DIK_A)
				&& !m_pGameInstance->Get_DIKeyState(DIK_D)
				&& m_iState != STATE_Lv25_SWRODGET_ST
				&& !m_bUltJump
				&& !m_bUltST
				&& !m_bUltLP
				&& !m_bUltED
				&& !m_bIsUpJumped && !m_bIsTakingDamage && !m_bIsInvincible)
			{
				m_bIsUpJumped = true;
				m_iState = STATE_JUMP;
				m_pGameInstance->PlaySoundEx("Jump.wav", SOUNDTYPE::JUMP, 0.4f);
			}
		}

		// 점프 + bool 값
		{
			// 정방향 bool 값
			if (m_bIsUpJumped)
			{
				m_fJumpTime += _fTimeDelta;

				m_pTransformCom->Go_Straight(_fTimeDelta, m_pBossStage_NavigationCom);

				if (m_fJumpTime < 0.3f)
				{
					m_fJumpSpeed -= 0.01f;
					_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

					vPos += XMVector3Normalize(vLook) * m_fJumpSpeed * 60.f * _fTimeDelta;

					if (true == m_pBossStage_NavigationCom->Is_Move(vPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
				}
				if (m_fJumpTime >= 0.6f)
				{
					m_fJumpTime = 0.f;
					m_fJumpSpeed = 0.2f;
					m_bIsUpJumped = false;
				}
			}
		}
	}

	// 링크가 데미지를 받고 있습니다
	if (m_iState == STATE_DMG_QUAKE && !m_bIsInvincible)
	{
		// 뒤로 물러가는 로직 
		{
			m_bIsTakingDamage = true;

			m_fDmgQuake -= 0.001f;
			m_fDmgBomb -= 0.005f;

			CZombie* pZombie;
			CBlueMoriblin* pBlueMoriblin;
			CStalfosOrange* pStalfosOrange;

			CCucco* pCucco;
			CBomberGreen* pBomberGreen;
			CChainIronBall* pChainIronBall;

			CCollider* pMonsterCollider = { nullptr };

			switch (m_iCollMonster)
			{
			case BLUEMORIBLIN:
				if (!m_bGetPosOnce)
				{
					pBlueMoriblin = dynamic_cast<CBlueMoriblin*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 0));
					pMonsterCollider = dynamic_cast<CCollider*>(pBlueMoriblin->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pBlueMoriblin->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case ZOMBIE:
				if (!m_bGetPosOnce)
				{
					pZombie = dynamic_cast<CZombie*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 1));
					pMonsterCollider = dynamic_cast<CCollider*>(pZombie->Get_Component(TEXT("Com_Monster_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pZombie->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case STALFOSORANGE:
				if (!m_bGetPosOnce)
				{
					pStalfosOrange = dynamic_cast<CStalfosOrange*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 2));
					//pMonsterCollider = dynamic_cast<CCollider*>(pStalfosOrange->Get_Component(TEXT("Com_Collider")));
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_vMonsterPos = dynamic_cast<CTransform*>(pStalfosOrange->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;

			case POTDEMON:
			{
				CPotDemon* pPotDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				CTransform* pTransform = dynamic_cast<CTransform*>(pPotDemon->Get_Component(TEXT("Com_Transform")));
				pMonsterCollider = dynamic_cast<CCollider*>(pPotDemon->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pTransform->Get_WorldMatrix().r[3].m128_f32[0], pTransform->Get_WorldMatrix().r[3].m128_f32[1], pTransform->Get_WorldMatrix().r[3].m128_f32[2] };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case DEMONKING_L:
				break;

			case DEMONKING_R:
				break;

			case CUCCO:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
				pMonsterCollider = dynamic_cast<CCollider*>(pCucco->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}


			case BOMBERGREEN:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pBomberGreen = dynamic_cast<CBomberGreen*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_BomberGreen")));
				pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pBomberGreen->Get_PartsWorldMatrix()._41, pBomberGreen->Get_PartsWorldMatrix()._42, pBomberGreen->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case CHAINIRONBALL:
			{
				CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
				pChainIronBall = dynamic_cast<CChainIronBall*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_ChainIronBall")));
				pMonsterCollider = dynamic_cast<CCollider*>(pChainIronBall->Get_Component(TEXT("Com_Collider")));
				if (!m_bGetPosOnce)
				{
					m_vMonsterPos = { pChainIronBall->Get_PartsWorldMatrix()._41, pChainIronBall->Get_PartsWorldMatrix()._42, pChainIronBall->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}
				break;
			}

			case RANSENGAN:
				/*if (!m_bGetPosOnce)
				{
					CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
					CCucco* pCucco = dynamic_cast<CCucco*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_Cucco")));
					pMonsterCollider = dynamic_cast<CCollider*>(pBomberGreen->Get_Component(TEXT("Com_Collider")));
					m_vMonsterPos = { pCucco->Get_PartsWorldMatrix()._41, pCucco->Get_PartsWorldMatrix()._42, pCucco->Get_PartsWorldMatrix()._43 };
					m_vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
					m_bGetPosOnce = true;
				}*/
				break;
			}

			_vector vDirToPlayer = XMVector3Normalize(m_vPlayerPos - m_vMonsterPos);

			//m_pTransformCom->LookAt_ForLandObject(m_vMonsterPos);


			if (m_fDmgQuake <= 0.f || m_fDmgBomb <= 0.f)
			{
				m_iState = STATE_IDLE;
				m_bGetPosOnce = false;
				m_bIsTakingDamage = false;
				m_bIsInvincible = true; // 데미지르 받고 직후 바로 무적인 상태로 만들기 위한 bool 값

				//m_fOverLap = 0.f;
				m_bGetOverlapOnce = false;
			}
			else // 몬스터 밀려나기
			{
				CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));

				if (m_iCollMonster == BOMBERGREEN
					|| m_iCollMonster == POTDEMON)
				{
					_vector vPlayerDir = vDirToPlayer * m_fDmgBomb;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pBossStage_NavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);


				}
				else
				{
					_vector	vPlayerDir = vDirToPlayer * m_fDmgQuake;

					m_vPlayerPos += vPlayerDir;
					XMVectorSetW(m_vPlayerPos, 1.f);

					if (true == m_pBossStage_NavigationCom->Is_Move(m_vPlayerPos))
						m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPlayerPos);
				}
				pCamera->Shake_Camera(_fTimeDelta, 0.05f, 0.05f);

			}
		}
	}
}

void CPlayer::Throw_FireBalls(_float _fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_Q) && !m_bIsFinishedShooting)
	{
		if (!m_bFireBallSound)
		{
			m_pGameInstance->PlaySoundEx("LSword_AttackCharging1.wav",SOUNDTYPE::JUMP, 0.5f);
			m_pGameInstance->PlaySoundEx("explosion.wav", SOUNDTYPE::ULT, 0.5f);
			m_bFireBallSound = true;
		}

		if (m_iFireBallCloneCount == 10)
		{
			m_pGameInstance->Clear_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_FireBall"));
			m_iFireBallCloneCount = -1;
		}

		m_iFireBallCloneCount++;
		m_bIsFinishedShooting = true;
		m_bIsSpecialSlash = true;

		CLandObject::LANDOBJ_DESC LandObjDesc{};
		{
			LandObjDesc.pNavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
			LandObjDesc.vecCells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation")))->Get_VecCell();

			// Mini GameA
			LandObjDesc.pGameA_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
			LandObjDesc.vecGameA_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_MiniGameA")))->Get_VecCell();

			// Mini GameB
			/*LandObjDesc.pGameB_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
			LandObjDesc.vecGameB_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_MiniGameB")))->Get_VecCell();*/
		}

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_FireBall"), TEXT("Prototype_GameObject_Weapon_FireBall"), &LandObjDesc)))
			return;
	}

	if (m_bIsFinishedShooting)
	{
		m_fFireBallTime += _fTimeDelta;

		if (m_fFireBallTime > 2.f)
		{
			m_bFireBallSound = false;
			m_bIsFinishedShooting = false;
			m_fFireBallTime = 0.f;
		}
	}

	if (m_bIsSpecialSlash)
	{
		m_iState = STATE_NORMAL_SLASH_ED;
	}
	
	// 여기서 GameA에 있는 몬스터 삭제해주기?
	/*CLayer* pStage = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame")));
	if (pStage == nullptr)
		return;

	CMap_GameA* pGameA = dynamic_cast<CMap_GameA*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), 0));
	if (pGameA == nullptr)
		return;

	if (4 == pGameA->Get_Stage_HitCount(CMap_GameA::STAGE_1))
	{
		for (auto iter = pStage->Get_List().begin(); iter != pStage->Get_List().end();)
		{
			iter = pStage->Get_List().erase(iter);
			++iter;
		}
	}*/
}

void CPlayer::SetUp_OnGames_Terrain()
{
	if (!m_bCancelOnTerrain && !m_bMiniGameA && !m_bIsBossStage && !m_bFirstView) // 3인칭 시점 메인맵
		SetUp_OnTerrain(m_pTransformCom, m_pNavigationCom);
	else if (!m_bCancelOnTerrain && m_bMiniGameA && !m_bIsBossStage && !m_bFirstView)	// 3인칭 시점 미니게임A
		SetUp_OnMiniGameA_Terrain(m_pTransformCom, m_pMiniGameA_NavigationCom);
	else if (!m_bCancelOnTerrain && m_bIsBossStage && !m_bMiniGameA && !m_bFirstView)	// 3인칭 시점 보스 스테이지
		SetUp_OnBossStage_Terrain(m_pTransformCom, m_pBossStage_NavigationCom);
	else if (m_bFirstView && !m_bCancelOnTerrain && !m_bMiniGameA && !m_bIsBossStage) // 1인칭 시점 메인맵
		SetUp_OnFirstView_Terrain(m_pTransformCom, m_pNavigationCom);
	else if (m_bFirstView && m_bMiniGameA && !m_bCancelOnTerrain && !m_bIsBossStage) // 1인칭 시점 미니게임A
		SetUp_OnFirstView_MiniGameA_Terrain(m_pTransformCom, m_pMiniGameA_NavigationCom);
	else if (m_bFirstView && m_bIsBossStage && !m_bCancelOnTerrain && !m_bMiniGameA) // 1인칭 시점 보스 스테이지
		SetUp_OnFirstView_BossStage_Terrain(m_pTransformCom, m_pBossStage_NavigationCom);
}

void CPlayer::PlaySound()
{
	//if()
	//m_pGameInstance->PlaySoundEx()
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC NaviDesc{};
	{
		NaviDesc.iStartCellIndex = 225;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL; 

	/* MiniGameA Com_Navigation */
	CNavigation::NAVI_DESC MiniGameA_NaviDesc{};
	{
		MiniGameA_NaviDesc.iStartCellIndex = 1;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_MiniGameA"),
		TEXT("Com_MiniGameA_Navigation"), reinterpret_cast<CComponent**>(&m_pMiniGameA_NavigationCom), &MiniGameA_NaviDesc)))
		return E_FAIL;

	/* BossStage Com_Navigation */
	CNavigation::NAVI_DESC BossStage_NaviDesc{};
	{
		BossStage_NaviDesc.iStartCellIndex = 6;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Boss_Map"),
		TEXT("Com_Boss_Map_Navigation"), reinterpret_cast<CComponent**>(&m_pBossStage_NavigationCom), &BossStage_NaviDesc)))
		return E_FAIL;

	/* Com_Collider */
	/*CBounding_AABB::AABB_DESC		BoundingDesc{};
	{
		BoundingDesc.vCenter = _float3(0.f, 2.f, 0.f);
		BoundingDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	}
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;*/


	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	CBody_Player::BODY_PLAYER_DESC BodyPlayerDesc{};
	{
		BodyPlayerDesc.pParentTransfrom = m_pTransformCom;
		BodyPlayerDesc.pPlayerState = &m_iState;
		BodyPlayerDesc.bChargingGauge = m_bCharging;
		BodyPlayerDesc.bUltJump = &m_bUltJump;
		BodyPlayerDesc.bCancelOnTerrain = &m_bCancelOnTerrain;
		BodyPlayerDesc.bUltST = &m_bUltST;
		BodyPlayerDesc.bUltLP = &m_bUltLP;
		BodyPlayerDesc.bUltED = &m_bUltED;
		BodyPlayerDesc.bIsSpecialSlash = &m_bIsSpecialSlash;
	}

	CPartObject* pBody_Player = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Body_Player"), &BodyPlayerDesc));
	if (nullptr == pBody_Player)
		return E_FAIL;

	m_mapPlayerParts.emplace(TEXT("Part_Body"), pBody_Player);

	CModel* pBodyModel = dynamic_cast<CModel*>(pBody_Player->Get_Component(TEXT("Com_Model")));
	if (nullptr == pBodyModel)
		return E_FAIL;

	CWeapon::WEAPON_DESC WeaponObjDesc{};
	{
		enum WEAPON_COLLIDER { WTYPE_SWORD, WTYPE_SHIELD, WTYPE_END };

		WeaponObjDesc.pParentTransfrom = m_pTransformCom;
		WeaponObjDesc.pSocketMatrix[WTYPE_SWORD] = pBodyModel->Get_CombinedBoneMatrixPtr("hand_L");
		WeaponObjDesc.pSocketMatrix[WTYPE_SHIELD] = pBodyModel->Get_CombinedBoneMatrixPtr("hand_R");
	}

	// 추후 파티클 생성위치의 행렬을 받아옵니다
	m_pParticle_SocketMatrix = pBodyModel->Get_CombinedBoneMatrixPtr("itemA_L_top");

	CPartObject* pWeapon = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponObjDesc));
	if (nullptr == pWeapon)
		return E_FAIL;

	m_mapPlayerParts.emplace(TEXT("Part_Weapon"), pWeapon);


	/////////////////
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY,TEXT("Layer_Camera")));
	if (nullptr == pWeapon)
		return E_FAIL;

	pCamera->Set_Player_PartsObject(pBody_Player);
	

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* _pArg, _uint _iLevel)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& Pair : m_mapPlayerParts)
		Safe_Release(Pair.second);
	m_mapPlayerParts.clear();

	Safe_Release(m_pParticlePoint);
	Safe_Release(m_pEffectSlash);
	Safe_Release(m_pEffectLanding);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pMiniGameA_NavigationCom);
	Safe_Release(m_pBossStage_NavigationCom);
}
