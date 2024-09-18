#include "stdafx.h"
#include "PotDemon.h"

#include "PotDemonPot.h"
#include "PotDemonKing.h"

// Ball
#include "Cucco.h"
#include "Rasengan.h"
#include "BomberGreen.h"
#include "ChainIronBall.h"

#include "Player.h"
#include "Animation.h"

CPotDemon::CPotDemon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CPotDemon::CPotDemon(const CPotDemon& _rhs)
    : CMonster(_rhs)
{
}

CComponent* CPotDemon::Get_PartObjectComponent(const wstring& _strPartObjTag, const wstring& _strComTag)
{
    auto iter = m_mapMonsterParts.find(_strPartObjTag);
    if (iter == m_mapMonsterParts.end())
        return nullptr;

    return iter->second->Get_Component(_strComTag);
}

CGameObject* CPotDemon::Get_PartObject(const wstring& _strPartObjTag)
{
	auto iter = m_mapMonsterParts.find(_strPartObjTag);
	if (iter == m_mapMonsterParts.end())
		return nullptr;

	return iter->second;
}

void CPotDemon::Delete_PartObject(const wstring& _strPartObjTag)
{
	auto iter = m_mapMonsterParts.find(_strPartObjTag);
	if (iter == m_mapMonsterParts.end())
		return;

	m_mapMonsterParts.erase(_strPartObjTag);
}

HRESULT CPotDemon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPotDemon::Initialize(void* _pArg)
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

	{
		_vector vecPos = { 3000.f, 41.5f, -198.7f, 1.0f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);
		m_pTransformCom->TurnTo(CTransform::TO_RIGHT, m_fTimeDelta);
	}

	return S_OK;
}

void CPotDemon::Priority_Tick(_float _fTimeDelta)
{
	for (auto& Pair : m_mapMonsterParts)
		(Pair.second)->Priority_Tick(_fTimeDelta);

	Invalidate_ColliderTransformationMatrix();
}

void CPotDemon::Tick(_float _fTimeDelta)
{
	// 플레이어의 위치 정보를 받아와서 카메라 위치로 세팅해주기
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	m_vPlayerPos = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);
	m_vDirToPlayer = m_vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fDisToPlayer = XMVector3Length(m_vDirToPlayer).m128_f32[0];

	CPotDemonKing* pDemonKing = dynamic_cast<CPotDemonKing*>(Get_PartObject(TEXT("Part_PotDemon_King")));
	_vector DemonKingPos = { pDemonKing->Get_PartsWorldMatrix()._41, pDemonKing->Get_PartsWorldMatrix()._42, pDemonKing->Get_PartsWorldMatrix()._43 };
	_vector DemonKingDir = m_vPlayerPos - DemonKingPos;
	_float DemonKingDis = XMVector3Length(DemonKingDir).m128_f32[0];

	// 거리안에 있다?  발견
	// 호리병 상태
	{
		if (m_bWalk)
		{

		}

		if (m_bHit)
		{
			if (m_iDemonKingState == POTDEMON_STATE_END)
				m_iPotDemonState = POT_STUN;
			else if (m_iDemonKingState == KING_ATTACK_FIRE)
				m_iDemonKingState = KING_CLONING_DAMAGE;
			else if (m_iDemonKingState == KING_CLONING_ED_WAIT_2)
				m_iDemonKingState = KING_CLONING_ULT_DAMAGE;
		}

		if (m_bLayDown)
		{
			/*if (m_iPotDemonState == POT_DEPOP)
			{
				m_bDemonKingComeOut = false;
			}

			if (!m_bDemonKingComeOut)
			{
				m_fTimeDelta += _fTimeDelta;
				m_iPotDemonState = POT_STUN_WAIT;

				if (m_fTimeDelta > 5.5f)
				{
					m_fTimeDelta = 0.f;
					m_bLayDown = false;
					m_bPhase2Pop = true;

				}
			}
			else
			{*/
				m_fTimeDelta += _fTimeDelta;
				m_iPotDemonState = POT_STUN_WAIT;

				if (m_fTimeDelta >= 1.f)
				{
					m_fTimeDelta = 0.f;
					m_bLayDown = false;
					m_bGetUp = true;
				}
			//}
		}

		if (m_bGetUp)
		{
			m_iPotDemonState = POT_GETUP;
		}
	}
	

	// Phase 1 데몬킹 출현 및 저글링
	{
		if (m_bPop)
		{
			m_bDemonKingComeOut = true;
			m_iPotDemonState = POT_POP_WAIT;
			m_iDemonKingState = KING_POP;

			if (!m_bGetUpSoundOnce)
			{
				m_pGameInstance->PlaySoundEx("PotDemon_Appear.wav", SOUNDTYPE::MON_APPEAR, 1.f);
				m_bGetUpSoundOnce = true;
			}
		}

		if (m_bWait)
		{
			m_fTimeDelta += _fTimeDelta;
			m_iDemonKingState = KING_WAIT;
			
			if (m_fTimeDelta >= 5.f)
			{
				m_pGameInstance->PlaySoundEx("Genie_WakeUp.mp3", SOUNDTYPE::MON_GUARD, 1.f);
				m_fTimeDelta = 0.f;
				m_bWait = false;
				m_bAtkFire = true;
			}
		}
		
		if (m_bAtkJuggling)
		{
			m_iPotDemonState = POT_ATTACK_JUGGLING;
			m_iDemonKingState = KING_ATTACK_JUGGLING;
		}

		if (m_bAtkFire || m_pGameInstance->Get_DIKeyState(DIK_B))
		{
			m_iPotDemonState = POT_ATTACK_FIRE;
			m_iDemonKingState = KING_ATTACK_FIRE;

			// 저글링 공 상태
			{
				m_iCuccoState = KING_ATTACK_FIRE;
				m_iChainIronBallState = KING_ATTACK_FIRE;
				m_iBomberGreenState = KING_ATTACK_FIRE;
			}
		}
		
		//if (m_pGameInstance->Get_DIKeyState(DIK_Q))
		//{
		//	m_bDepop = true;
		//}
		// 이건 순간이동을 사용할시에 사용되는 if조건문입니다
		//if (m_bDepop)
		//{	// 저글링 공 없애기
		//	{
		//		m_iCuccoState = POTDEMON_STATE_END;
		//		m_iChainIronBallState = POTDEMON_STATE_END;
		//		m_iBomberGreenState = POTDEMON_STATE_END;
		//	}

		//	m_fTimeDelta += _fTimeDelta;

		//	m_iPotDemonState = POT_DEPOP;
		//	m_iDemonKingState = KING_DEPOP;

		//	if (m_fTimeDelta > 1.5f)
		//	{
		//		m_fTimeDelta = 0.f;
		//		m_bLayDown = true;
		//		m_bDepop = false;
		//		//m_bDepop = false;
		//		//m_bPhase2Pop = true;
		//	}
		//}

		//if (m_bAtkUlt)
		//{
		//	// 저글링 공 없애기
		//	{
		//		m_iCuccoState = POTDEMON_STATE_END;
		//		m_iChainIronBallState = POTDEMON_STATE_END;
		//		m_iBomberGreenState = POTDEMON_STATE_END;
		//	}


		//}

		// 순간이동을 위한 애니메이션입니다
		//if (m_bDepop)
		//{	
		//	m_fTimeDelta += _fTimeDelta;

		//	m_iPotDemonState = POT_DEPOP;
		//	m_iDemonKingState = KING_DEPOP;

		//	if (m_fTimeDelta > 1.5f)
		//	{
		//		m_fTimeDelta = 0.f;
		//		m_bLayDown = true;
		//		m_bDepop = false;
		//		//m_bDepop = false;
		//		//m_bPhase2Pop = true;
		//	}
		//}

		if (m_bDepop)
		{
			m_iPotDemonState = POT_ATTACK_JUGGLING;
			m_iDemonKingState = KING_CLONING_DEAD;

			{
				m_iCuccoState = KING_CLONING_DEAD;
				m_iChainIronBallState = KING_CLONING_DEAD;
				m_iBomberGreenState = KING_CLONING_DEAD;
			}

			if (!m_bPlayDeadSoundOnce)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::BGM);
				//m_pGameInstance->PlaySoundEx("Geni_Dead.wav", SOUNDTYPE::MON_DEAD, 1.0f);
				m_pGameInstance->PlaySoundEx("1-40 Puzzle Solved Jingle.mp3", SOUNDTYPE::BGM, 0.5f);
				m_bPlayDeadSoundOnce = true;
			}

		}

		if (m_bDead)
		{
			m_iPotDemonState = POT_DEPOP;
			m_iDemonKingState = KING_DEPOP;

			if (!m_bPlayerBGMOnce)
			{
				m_pGameInstance->StopSound(SOUNDTYPE::BGM);
				m_pGameInstance->PlaySoundEx("PotDemon_Appear.wav", SOUNDTYPE::MON_APPEAR, 1.f);
				m_bPlayerBGMOnce = true;
			}
		}
	}
	


	// 저글링 공격 볼 3개 상태
	{
		if (m_bOnFireCucco)
		{
			wstring wstrCloneCount = to_wstring(m_iCucco_CloneCount);
		
			CCucco::CUCCO_DESC Cucco_Desc{};
			{
				//BomberGreen_Desc.pSocketMatrix = pModel->Get_CombinedBoneMatrixPtr("fire_c");
 				Cucco_Desc.pParentTransfrom = m_pTransformCom;
				Cucco_Desc.bOnFireCucco = &m_bOnFireCucco;
				Cucco_Desc.iCuccoState = &m_iCuccoState;
				Cucco_Desc.iCucco_CloneCount = m_iCucco_CloneCount;
			}
 			CPotDemonPartObject* pCucco = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_Cucco"), &Cucco_Desc));

			m_mapMonsterParts.emplace(TEXT("Part_PotDemon_Cucco_Clone") + wstrCloneCount, pCucco);

			m_bOnFireCucco = false;
		}

		if (m_bOnFireBomberGreen || m_pGameInstance->Get_DIKeyState(DIK_K))
		{
			m_wstrCloneCount = to_wstring(m_iBomberGreen_CloneCount);

			CBomberGreen::BOMBERGREEN_DESC BomberGreen_Desc{};
			{
				//BomberGreen_Desc.pSocketMatrix = pModel->Get_CombinedBoneMatrixPtr("fire_c");
				BomberGreen_Desc.pParentTransfrom = m_pTransformCom;
				BomberGreen_Desc.bOnFireBomberGreen = &m_bOnFireBomberGreen;
				BomberGreen_Desc.iBomberGreenState = &m_iBomberGreenState;
				BomberGreen_Desc.iBomberGreen_CloneCount = m_iBomberGreen_CloneCount;
			}
			CPotDemonPartObject* pBomberGreen = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_BomberGreen"), &BomberGreen_Desc));

			m_mapMonsterParts.emplace(TEXT("Part_PotDemon_BomberGreen_Clone") + m_wstrCloneCount, pBomberGreen);

			m_bOnFireBomberGreen = false;
		}

		if (m_bOnFireChainIronBall)
		{
			wstring wstrCloneCount = to_wstring(m_iChainIronBall_CloneCount);

			CChainIronBall::CHAINIRONBALL_DESC ChainIronBall_Desc{};
			{
				//BomberGreen_Desc.pSocketMatrix = pModel->Get_CombinedBoneMatrixPtr("fire_c");
				ChainIronBall_Desc.pParentTransfrom = m_pTransformCom;
				ChainIronBall_Desc.bOnFireChainIronBall = &m_bOnFireChainIronBall;
				ChainIronBall_Desc.iChainIronBallState = &m_iChainIronBallState;
				ChainIronBall_Desc.iChainIronBall_CloneCount = m_iChainIronBall_CloneCount;
			}
			CPotDemonPartObject* pChainIronBall = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_ChainIronBall"), &ChainIronBall_Desc));
			
			m_mapMonsterParts.emplace(TEXT("Part_PotDemon_ChainIronBall_Clone") + wstrCloneCount, pChainIronBall);

			m_bOnFireChainIronBall = false;
		}

		if (m_bOnFireRasengan || m_pGameInstance->Get_DIKeyState(DIK_L))
		{
			//CModel* pModel = dynamic_cast<CModel*>(Get_PartObjectComponent(TEXT("Part_PotDemon_King"), TEXT("Com_Model")));
			
			//m_iRasengan_CloneCount++;

			wstring wstrCloneCount = to_wstring(m_iRasengan_CloneCount);

			CRasengan::RASENGAN_DESC Rasengan_Desc{};
			{
				// Rasengan이 붙을 DemonKing 뼈의 행렬을 건내줍니다
				//Rasengan_Desc.pSocketMatrix = pModel->Get_CombinedBoneMatrixPtr("fire_c");
				// Rasengan에게 부모 행렬을 건내줍니다 
				Rasengan_Desc.pParentTransfrom = m_pTransformCom;
				// DemonKing이 Rasengan을 던졌는지 안던졌는지를 판단하는 bool 값입니다
				Rasengan_Desc.bOnFireRasengan = &m_bOnFireRasengan;
				// Rasengan의 애니메이션을 지정해주기 위한 int 변수입니다
				Rasengan_Desc.iRasenganState = &m_iRasenganState;
				// Rasengan이 저글링용인지, 던지기용인지 구분하기 위한 uint 변수입니다
				Rasengan_Desc.iRasengan_CloneCount = m_iRasengan_CloneCount;
			}
			CPotDemonPartObject* pRasengan = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_Rasengan"), &Rasengan_Desc));

			m_mapMonsterParts.emplace(TEXT("Part_PotDemon_Rasengan")+ wstrCloneCount, pRasengan);
		}
	}
	
	// Phase 2 진입
	{
		if (m_bPhase2Pop || m_pGameInstance->Get_DIKeyState(DIK_P))
		{
			m_iDemonKingState = KING_PHASE_2_ST;
		}

		if (m_bPhase2Wait)
		{
			m_fTimeDelta += _fTimeDelta;
			m_iDemonKingState = KING_PHASE_2_WAIT;

			if (m_fTimeDelta > 1.f)
			{
				m_bPhase2Disappear = true;
				m_fTimeDelta = 0.f;
			}
		}

		if (m_bPhase2Disappear)
		{
			m_iDemonKingState = KING_PHASE_2_ED;

			_vector RandomPos = { 3.44f, 10.5f, 59.f, 1.0f };
			
			pDemonKing->Set_State(RandomPos);
			dynamic_cast<CTransform*>(pDemonKing->Get_Component(TEXT("Com_Transform")))->LookAt(m_vPlayerPos);

		}

		if (m_bPhase2Appear)
		{
			m_iDemonKingState = KING_CLONING_APPEAR;
		}

		// 난사
		if (m_bPhase2Punch)
		{
			m_bDemonKingComeOut = true;
			m_iDemonKingState = KING_CLONING_ATK_PUNCH;

			if(DemonKingDis >= 5.f)
			{
				m_bPhase2Punch = false;
				m_bPhase2PunchED = true;
			}
		}

		if (m_bPhase2PunchED)
		{
			m_iDemonKingState = KING_CLONING_ATK_PUNCH_ED;
		}


		// 나선환 던지기
		if (m_bPhase2ThrowST || m_pGameInstance->Get_DIKeyState(DIK_R))
		{
			// 조건을 만족하지 않는 키-값 쌍 제거
			/*auto iter = m_mapMonsterParts.begin();
			if (m_mapMonsterParts.find(TEXT("Part_PotDemon_Pot")) != m_mapMonsterParts.end()) {
				iter++;
			}
			if (m_mapMonsterParts.find(TEXT("Part_PotDemon_King")) != m_mapMonsterParts.end()) {
				iter++;
			}
			while (iter != m_mapMonsterParts.end())
			{
					m_mapMonsterParts.erase(iter->first);
			}*/

			auto iter = m_mapMonsterParts.begin();

			while (iter != m_mapMonsterParts.end())
			{
				if (iter->first == TEXT("Part_PotDemon_Pot") || iter->first == TEXT("Part_PotDemon_King")){
					++iter;
				}
				else{
					iter = m_mapMonsterParts.erase(iter);
				}
			}

			m_iCuccoState = KING_ATK_ULT;
			m_iChainIronBallState = KING_ATK_ULT;
			m_iBomberGreenState = KING_ATK_ULT;
			m_iRasenganState = KING_ATK_ULT;

			m_fTimeDelta += _fTimeDelta;
			m_iDemonKingState = KING_CLONING_ED_WAIT;

			//여기서 원기옥 사이즈 키우면서 기다리기
			// 특정 조건일때, 던지기 신호 땅  +  이때 PotDemon 콜라이더 나오면서, 그 범위 안에 있으면 다치는 느낌으로 간다
			if(m_fTimeDelta > 5.f)
			{
				m_fTimeDelta = 0.f;
				m_bPhase2ThrowST = false;
				m_bPhase2ThrowLP = true;
			}
		}

		if (m_bPhase2ThrowLP)
		{
			m_iDemonKingState = KING_CLONING_ED_THROW;
		}

		// 이건 순간이동 애니메이션을 사용할때
		/*if (m_bPhase2ThrowED)
		{
			m_fTimeDelta += _fTimeDelta;
			m_iDemonKingState = KING_CLONING_ED_WAIT_2;

			if (m_fTimeDelta > 1.5f)
			{
				m_bPhase2ThrowED = false;
				m_bPhase2Disappear = true;
			}
		}*/

		if (m_bPhase2ThrowED)
		{
			m_fTimeDelta += _fTimeDelta;
			m_iDemonKingState = KING_CLONING_ED_WAIT_2;

			if (m_fTimeDelta > 5.5f)
			{
				m_fTimeDelta = 0.f;
				m_bPhase2ThrowED = false;
				m_bPhase2ThrowST = true;
			}
		}

		if (m_bPhase2Damage)
		{

		}

		if (m_bPhase2Dead)
		{

		}
	}



	// 어떤 상태도 아닐 때
	//if (m_iPotDemonState == POTDEMON_STATE_END
	//	/*&& m_iDemonKingState == POTDEMON_STATE_END*/)
	//{
	//	m_iPotDemonState = POT_WAIT;
	//}

	for (auto& Pair : m_mapMonsterParts)
		(Pair.second)->Tick(_fTimeDelta);
}

void CPotDemon::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	for (auto& Pair : m_mapMonsterParts)
		(Pair.second)->Late_Tick(_fTimeDelta);

	SetUp_OnBossStage_Terrain(m_pTransformCom, m_pNavigationCom);

#ifdef _DEBUG
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#endif
}

HRESULT CPotDemon::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
    return S_OK;
}

void CPotDemon::Collision_ToPlayer()
{
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));

	// 현재 플레이어의 애니메이션 상태를 가져옵니다
	if (nullptr != pModel)
		iCurAnim = pModel->Get_CurAnimation();

	if (Collision_ToPlayerBody())
	{
		Handle_Spheres_Sliding(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
		//m_pGameInstance->PlaySoundEx("Link_Pull.wav", SOUNDTYPE::MON_PUSH, 0.4f);
	}

	if (Collision_ToPlayerSword()
		&& (STATE_SLASH == iCurAnim
			|| STATE_SLASH_LP == iCurAnim
			|| STATE_NORMAL_SLASH_ED == iCurAnim)
		&& (m_iDemonKingState == KING_ATTACK_FIRE 
			|| m_iDemonKingState == KING_CLONING_DAMAGE
			|| m_iDemonKingState == KING_CLONING_ED_WAIT_2
			|| m_iDemonKingState == KING_CLONING_ULT_DAMAGE))
	{
		if (iCurAnim != -1)
		{
			pModel->Get_VecAnims()[iCurAnim]->Set_TrackPosition(0.f);
			pModel->Get_VecAnims()[STATE_DMG_QUAKE]->Set_TrackPosition(0.f);
		}

		pPlayer->Set_CollMonster(CPlayer::POTDEMON);
		pPlayer->Set_CurState(STATE_DMG_QUAKE);

		m_bHit = true;
		m_bAtkFire = false;
		m_bPhase2ThrowST = false; 
		m_bPhase2ThrowED = false;
		m_pGameInstance->PlaySoundEx("PotDemon_Damage.wav", SOUNDTYPE::MON_PUSH, 0.4f);
	}
}

_bool CPotDemon::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CPotDemon::Collision_ToPlayerSword()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Sword_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CPotDemon::Collision_ToPlayerShield()
{
	return _bool();
}

void CPotDemon::Invalidate_ColliderTransformationMatrix()
{
	// 내 자신의 월드행렬에 부모의 월드행렬을 곱함으로써, 최종 월드 행렬을 가진다
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

HRESULT CPotDemon::Add_Components()
{
	/* Com_Navigation */
	CNavigation::NAVI_DESC NaviDesc{};
	{
		NaviDesc.iStartCellIndex = 32;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Boss_Map"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* Body Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
		{
			BodyBoundingDesc.fRadius = 0.8f;
			BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
			return E_FAIL;
	}

    return S_OK;
}

HRESULT CPotDemon::Add_PartObjects()
{
	CPotDemonPot::POT_DESC PotDesc{};
	{
		PotDesc.pParentTransfrom = m_pTransformCom;
		PotDesc.pPotDemonState = &m_iPotDemonState;
		PotDesc.pNavTranform = m_pBossStage_NavTransform;
		PotDesc.pNavigationCom = m_pNavigationCom;
		PotDesc.vecCells = m_vecCells;
		
		PotDesc.bDemonKingComeOut = &m_bDemonKingComeOut;
		PotDesc.bWait = &m_bWait;
		PotDesc.bPop = &m_bPop;
		PotDesc.bDepop = &m_bDepop;
		PotDesc.bHit = &m_bHit;
		PotDesc.iHitCount = &m_iHitCount;
		PotDesc.bGetUp = &m_bGetUp;
		PotDesc.bLayDown = &m_bLayDown;
		PotDesc.bAtkFire = &m_bAtkFire;
		PotDesc.bAtkJuggling = &m_bAtkJuggling;
	}

	CPotDemonPartObject* pPotDemon_Pot = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_Pot"), &PotDesc));
	if (nullptr == pPotDemon_Pot)
		return E_FAIL;

	m_mapMonsterParts.emplace(TEXT("Part_PotDemon_Pot"), pPotDemon_Pot);

	CModel* pPotModel = dynamic_cast<CModel*>(pPotDemon_Pot->Get_Component(TEXT("Com_Model")));
	if (nullptr == pPotModel)
		return E_FAIL;

	m_DemonKingWorldMatrix = pPotModel->Get_CombinedBoneMatrixPtr("spine_e");
	
	CPotDemonKing::DEMON_KING_DESC DemonKing_Desc{};
	{
		enum DEMON_KING_COLLIDER { DKC_ROOT, DKC_HAND_L, DKC_HAND_R, DKC_FB_A, DKC_FB_B, DKC_FB_C, DKC_END };

		DemonKing_Desc.iDemonKingState = &m_iDemonKingState;
		DemonKing_Desc.pParentTransfrom = m_pTransformCom;
		DemonKing_Desc.pSocketMatrix[DKC_ROOT] = m_DemonKingWorldMatrix;

		DemonKing_Desc.bHit = &m_bHit;
		DemonKing_Desc.iHitCount = &m_iHitCount;

		DemonKing_Desc.bWait = &m_bWait;
		DemonKing_Desc.bPop = &m_bPop;
		DemonKing_Desc.bAtkJuggling = &m_bAtkJuggling;
		DemonKing_Desc.bAtkFire = &m_bAtkFire;
		DemonKing_Desc.bDepop = &m_bDepop;
		DemonKing_Desc.bDead = &m_bDead;

		// Demon King
		DemonKing_Desc.bPhase2Pop = &m_bPhase2Pop;
		DemonKing_Desc.bPhase2Wait = &m_bPhase2Wait;
		DemonKing_Desc.bPhase2Disappear = &m_bPhase2Disappear;
		DemonKing_Desc.bPhase2Appear = &m_bPhase2Appear;
		DemonKing_Desc.bPhase2Punch = &m_bPhase2Punch;
		DemonKing_Desc.bPhase2PunchED = &m_bPhase2PunchED;
		DemonKing_Desc.bPhase2ThrowST = &m_bPhase2ThrowST;
		DemonKing_Desc.bPhase2ThrowLP = &m_bPhase2ThrowLP;
		DemonKing_Desc.bPhase2ThrowED = &m_bPhase2ThrowED;
		DemonKing_Desc.bPhase2Damage = &m_bPhase2Damage;
		DemonKing_Desc.bPhase2Dead = &m_bPhase2Dead;

		DemonKing_Desc.bOnFireCucco = &m_bOnFireCucco;
		DemonKing_Desc.bOnFireChainIronBall = &m_bOnFireChainIronBall;
		DemonKing_Desc.bOnFireBomberGreen = &m_bOnFireBomberGreen;
		DemonKing_Desc.bOnFireRasengan = &m_bOnFireRasengan;

		DemonKing_Desc.iChainIronBall_CloneCount = &m_iChainIronBall_CloneCount;
		DemonKing_Desc.iBomberGreen_CloneCount = &m_iBomberGreen_CloneCount;
		DemonKing_Desc.iCucco_CloneCount = &m_iCucco_CloneCount;
		DemonKing_Desc.iRasengan_CloneCount = &m_iRasengan_CloneCount;
	}

	CPotDemonPartObject* pPotDemon_King = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_King"), &DemonKing_Desc));
	if (nullptr == pPotDemon_King)
		return E_FAIL;

	m_mapMonsterParts.emplace(TEXT("Part_PotDemon_King"), pPotDemon_King);

	CModel* pDemon_King = dynamic_cast<CModel*>(pPotDemon_King->Get_Component(TEXT("Com_Model")));
	if (nullptr == pPotModel)
		return E_FAIL;

	// Balls
	{
		CBomberGreen::BOMBERGREEN_DESC BomberGreen_Desc{};
		{
			// BomberGreen이 붙을 DemonKing 뼈의 행렬
			BomberGreen_Desc.pSocketMatrix = pDemon_King->Get_CombinedBoneMatrixPtr("fire_a");
			// BomberGreen에게 부모 행렬을 건내줍니다 
			BomberGreen_Desc.pParentTransfrom = m_pTransformCom;
			// DemonKing이 BomberGreen을 던졌는지 안던졌는지를 판단하는 bool 값입니다
			BomberGreen_Desc.bOnFireBomberGreen = &m_bOnFireBomberGreen;
			// BomberGreen의 애니메이션을 지정해주기 위한 int 변수입니다
			BomberGreen_Desc.iBomberGreenState = &m_iBomberGreenState;
			// BomberGreen이 저글링용인지, 던지기용인지 구분하기 위한 uint 변수입니다
			BomberGreen_Desc.iBomberGreen_CloneCount = 0;
		}
		CPotDemonPartObject* pBomberGreen = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_BomberGreen"), &BomberGreen_Desc));
		if (nullptr == pBomberGreen)
			return E_FAIL;

		m_mapMonsterParts.emplace(TEXT("Part_PotDemon_BomberGreen"), pBomberGreen);


		CChainIronBall::CHAINIRONBALL_DESC ChainIronBall_Desc{};
		{
			// ChainIronBall이 붙을 DemonKing 뼈의 행렬을 건내줍니다
			ChainIronBall_Desc.pSocketMatrix = pDemon_King->Get_CombinedBoneMatrixPtr("fire_b");
			// ChainIronBall에게 부모 행렬을 건내줍니다 
			ChainIronBall_Desc.pParentTransfrom = m_pTransformCom;
			// DemonKing이 ChainIronBall을 던졌는지 안던졌는지를 판단하는 bool 값입니다
			ChainIronBall_Desc.bOnFireChainIronBall = &m_bOnFireChainIronBall;
			// ChainIronBall의 애니메이션을 지정해주기 위한 int 변수입니다
			ChainIronBall_Desc.iChainIronBallState = &m_iChainIronBallState;
			// ChainIronBall이 저글링용인지, 던지기용인지 구분하기 위한 uint 변수입니다
			ChainIronBall_Desc.iChainIronBall_CloneCount = 0;
		}
		CPotDemonPartObject* pChainIronBall = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_ChainIronBall"), &ChainIronBall_Desc));
		if (nullptr == pChainIronBall)
			return E_FAIL;

		m_mapMonsterParts.emplace(TEXT("Part_PotDemon_ChainIronBall"), pChainIronBall);


		CCucco::CUCCO_DESC Cucco_Desc{};
		{
			// ChainIronBall이 붙을 DemonKing 뼈의 행렬을 건내줍니다
			Cucco_Desc.pSocketMatrix = pDemon_King->Get_CombinedBoneMatrixPtr("fire_c");
			// ChainIronBall에게 부모 행렬을 건내줍니다 
			Cucco_Desc.pParentTransfrom = m_pTransformCom;
			// DemonKing이 ChainIronBall을 던졌는지 안던졌는지를 판단하는 bool 값입니다
			Cucco_Desc.bOnFireCucco = &m_bOnFireCucco;
			// ChainIronBall의 애니메이션을 지정해주기 위한 int 변수입니다
			Cucco_Desc.iCuccoState = &m_iCuccoState;
			// ChainIronBall이 저글링용인지, 던지기용인지 구분하기 위한 uint 변수입니다
			Cucco_Desc.iCucco_CloneCount = 0;
		}
		CPotDemonPartObject* pCucco = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_Cucco"), &Cucco_Desc));
		if (nullptr == pCucco)
			return E_FAIL;

		m_mapMonsterParts.emplace(TEXT("Part_PotDemon_Cucco"), pCucco);

		CModel* pModel = dynamic_cast<CModel*>(Get_PartObjectComponent(TEXT("Part_PotDemon_King"), TEXT("Com_Model")));

		CRasengan::RASENGAN_DESC Rasengan_Desc{};
		{
			// Rasengan이 붙을 DemonKing 뼈의 행렬을 건내줍니다
			Rasengan_Desc.pSocketMatrix = pDemon_King->Get_CombinedBoneMatrixPtr("fire_c");
			// Rasengan에게 부모 행렬을 건내줍니다 
			Rasengan_Desc.pParentTransfrom = m_pTransformCom;
			// DemonKing이 Rasengan을 던졌는지 안던졌는지를 판단하는 bool 값입니다
			Rasengan_Desc.bOnFireRasengan = &m_bOnFireRasengan;
			// Rasengan의 애니메이션을 지정해주기 위한 int 변수입니다
			Rasengan_Desc.iRasenganState = &m_iRasenganState;
			// Rasengan이 저글링용인지, 던지기용인지 구분하기 위한 uint 변수입니다
			Rasengan_Desc.iRasengan_CloneCount = 0;
		}
		CPotDemonPartObject* pRasengan = dynamic_cast<CPotDemonPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_PotDemon_Rasengan"), &Rasengan_Desc));

		m_mapMonsterParts.emplace(TEXT("Part_PotDemon_Rasengan"), pRasengan);
	}
	
    return S_OK;
}

void CPotDemon::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{ ///////////////// Player 중점과 반지금///////////////////////////////////////////////////
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(_pGameObject);
	if (nullptr == pPlayer)
		return;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return;

	CBounding_Sphere* pBoundingSphere = dynamic_cast<CBounding_Sphere*>(pPlayerCollider->Get_Bounding());
	if (nullptr == pBoundingSphere)
		return;

	_float vPlayerRadius = ((BoundingSphere*)(pBoundingSphere->Get_Bounding()))->Radius;
	_float3 vPlayerCenter = ((BoundingSphere*)(pBoundingSphere->Get_Bounding()))->Center;
	///////////////////////////////////////////////////////////////////////////////////////////

	CBounding_Sphere* pMonsterSphere = dynamic_cast<CBounding_Sphere*>(m_pColliderCom->Get_Bounding());
	if (nullptr == pMonsterSphere)
		return;

	_float vMonsterRadius = ((BoundingSphere*)(pMonsterSphere->Get_Bounding()))->Radius;
	_float3 vMonsterCenter = ((BoundingSphere*)(pMonsterSphere->Get_Bounding()))->Center;
	///////////////////////////////////////////////////////////////////////////////////////////////

	// 서로 바라보는두 방향벡터 구해줌
	_vector vDirToPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayerCenter) - XMLoadFloat3(&vMonsterCenter));
	_vector vDirToMonster = XMVector3Normalize(XMLoadFloat3(&vMonsterCenter) - XMLoadFloat3(&vPlayerCenter));

	//vDirToPlayer = XMVectorSet(XMVectorGetX(vDirToPlayer), XMVectorGetY(vDirToPlayer), 0.0f, 0.0f);
   // vDirToMonster = XMVectorSet(XMVectorGetX(vDirToMonster), XMVectorGetY(vDirToMonster), 0.0f, 0.0f);

	_float fDis = vPlayerRadius + vMonsterRadius;
	_float fOverLap = fDis - XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPlayerCenter) - XMLoadFloat3(&vMonsterCenter)));

	// 플레이어 밀려나기
	{
		_vector vPlayerDir = vDirToPlayer * (fOverLap * 0.5f);
		_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		vPlayerPos += vPlayerDir;
		dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}

	// 몬스터 밀려나기
	{
		_vector vMonsterDir = vDirToMonster * (fOverLap * 0.5f);
		_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vMonsterPos += vMonsterDir;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
	}
}

CPotDemon* CPotDemon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPotDemon* pInstance = new CPotDemon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPotDemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotDemon::Clone(void* _pArg, _uint _iLevel)
{
	CPotDemon* pInstance = new CPotDemon(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPotDemon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotDemon::Free()
{
	__super::Free();

	for (auto& Pair : m_mapMonsterParts)
		Safe_Release(Pair.second);
	m_mapMonsterParts.clear();

	Safe_Release(m_pColliderCom);
}
