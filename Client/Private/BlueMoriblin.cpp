#include "stdafx.h"
#include "BlueMoriblin.h"

// Engine
#include "Model.h"
#include "Player.h"
#include "Animation.h"
#include "GameInstance.h"

#include "Bounding.h"
#include "Bounding_Sphere.h"
#include "Effect_Texture.h"

CBlueMoriblin::CBlueMoriblin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CBlueMoriblin::CBlueMoriblin(const CBlueMoriblin& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CBlueMoriblin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlueMoriblin::Initialize(void* _pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)_pArg;
	{
		pGameObjectDesc->fSpeedPerSec = 1.f;
		pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.f);
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_matrix WorldMatrix = XMMatrixSet
	(0.0f, 0.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	-45.04f, 20.f, 68.6f, 1.0f);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10, 3.f, rand() % 10, 1.f));
	m_pModelCom->Set_Init_Animation(0);

	m_pEffect_Blood = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect")));
	if (nullptr == m_pEffect_Blood)
		return E_FAIL;

	m_pEffect_Metal = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), 1));
	if (nullptr == m_pEffect_Metal)
		return E_FAIL;

	Safe_AddRef(m_pEffect_Blood);
	Safe_AddRef(m_pEffect_Metal);

	return S_OK;
}

void CBlueMoriblin::Priority_Tick(_float _fTimeDelta)
{
	Invalidate_ColliderTransformationMatrix();
}

void CBlueMoriblin::Tick(_float _fTimeDelta)
{
	// 플레이어의 위치 정보를 받아와서 카메라 위치로 세팅해주기
	{
		CGameObject* pGameObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_vPlayerPos = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

		m_vDirToPlayer = m_vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_fDisToPlayer = XMVector3Length(m_vDirToPlayer).m128_f32[0];
	}

	// 거리안에 있다?  발견
	if (m_fDisToPlayer <= 8.f && !m_bDead)
	{
		Rotate_Monster(&m_bFind);

		//// "적 발견!!" 전까지는 가만히 서서 Y축 기준으로 이동하지 않고, 자전 회전만 해주기
		if (!m_bFind)
		{
			_vector vAxisY = { 0.f, 1.f, 0.f, 0.f };
			m_pTransformCom->Turn(vAxisY, _fTimeDelta);

			m_pGameInstance->PlaySoundEx("Moriblin_Vo_Search00.wav", SOUNDTYPE::MON_GUARD, 1.f);
		}
	} 
	else if(!m_bDead) // 반복 재생
	{
		m_bAvoidSuddenTurn = false;
		m_bFindBefore = false;
		m_bFind = false;
		m_bWalk = false;
		m_bAttack = false;
		m_bGuard = false;
		m_bHit = false;
		m_bStagger = false;
		m_bDead = false;
		m_bDeadSoundOnce = false;

		m_pModelCom->Set_Animation(0);
		m_pModelCom->Play_Animation(_fTimeDelta, true);
	}

	if (m_bFind && !m_bWalk && !m_bAttack && !m_bGuard && !m_bHit && !m_bStagger && !m_bDead)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[1]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[1]->Get_TrackPosition();

		m_pModelCom->Set_Animation(1);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >= dDuration)
		{
			m_bWalk = true;
			m_bFind = false;
		}
	}
	if (m_bWalk && !m_bDead)
	{
		m_pModelCom->Set_Animation(2);
		m_pModelCom->Play_Animation(_fTimeDelta, true);

		// 걸으면서 플레이어 위치에 따라서 방향을 바꿔주기

		if (!m_bFindBefore)
		{
			m_pTransformCom->LookAt(m_vPlayerPos);
			m_pTransformCom->MoveTo(m_vPlayerPos, 2.f, _fTimeDelta);
		}
		else
		{
			if (m_bAvoidSuddenTurn)
			{
				m_pTransformCom->LookAt(m_vPlayerPos);
				m_pTransformCom->MoveTo(m_vPlayerPos, 2.f, _fTimeDelta);
			}
			else
			{
				Rotate_Monster(&m_bAvoidSuddenTurn);

				_vector vAxisY = { 0.f, 1.f, 0.f, 0.f };
				m_pTransformCom->Turn(vAxisY, _fTimeDelta);
			}
		}
		

		m_pGameInstance->PlaySoundEx("Moriblin_Walk (2).wav", SOUNDTYPE::MON_WALK, 1.f);
		

		if (m_fDisToPlayer <= 2.f)
		{
			m_bAttack = true;
			m_bWalk = false;
		}
	}
	else if (m_bAttack && !m_bDead)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[3]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[3]->Get_TrackPosition();

		m_pModelCom->Set_Animation(3);
		m_pModelCom->Play_Animation(_fTimeDelta, false);
		
		if (dTrackPosition >= dDuration)
		{
			if (m_fDisToPlayer > 2.f && m_fDisToPlayer <= 5.f)
			{
				m_bWalk = true;
				m_bAttack = false;
				m_bFindBefore = true; 
				m_bAvoidSuddenTurn = false;
			}
		}

		
	}
	else if (m_bGuard && !m_bDead)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[4]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[4]->Get_TrackPosition();

		m_pModelCom->Set_Animation(4);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >= dDuration)
		{
			m_bGuard = false;

			if (m_fDisToPlayer <= 2.f)
			{
				m_bAttack = true;
			}
			else
				m_bWalk = true;
		}
	}
	else if (m_bHit && !m_bDead)
	{
		m_bFind = false;
		m_bWalk = false;
		m_bAttack = false;
		m_bGuard = false;
		m_bDead = false;

		_double dDuration = m_pModelCom->Get_VecAnims()[5]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[5]->Get_TrackPosition();

		m_pModelCom->Set_Animation(5);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >= dDuration)
		{
			m_iHitCount++;
			m_bWalk = true;
			m_bHit = false;
		}
		if (m_iHitCount == 6)
		{
			m_iHitCount = 0;
			m_bHit = false;
			m_bDead = true;
		}
	}
	else if(m_bStagger && !m_bDead)
	{
		// 뒤로 물러가는 로직 
		{
			_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vDirToPlayer = XMVector3Normalize(vMonsterPos - m_vPlayerPos);

			// 몬스터 밀려나기
			{
				_vector vMonsterDir = vDirToPlayer * m_fStagger;
				vMonsterPos += vMonsterDir;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
			}
		}
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[6]->Get_TrackPosition(); 

		m_pModelCom->Set_Animation(6);
		m_pModelCom->Get_VecAnims()[6]->Set_TickPerSec(40.f);

		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >= 40)
		{
			m_bStagger = false;
		}
	}
	else if (m_bDead)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[7]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[7]->Get_TrackPosition();

		m_pModelCom->Set_Animation(7);
		m_pModelCom->Play_Animation(_fTimeDelta, false);
	
		if (!m_bDeadSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("Moriblin_Vo_Die01.wav", SOUNDTYPE::MON_DEAD, 1.f);
			m_bDeadSoundOnce = true;
		}
	}
}

void CBlueMoriblin::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	SetUp_OnTerrain(m_pTransformCom, m_pNavigationCom);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CBlueMoriblin::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

#ifdef _DEBUG
	for (size_t i = 0; i < TYPE_END; i++)
	{
		m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

void CBlueMoriblin::Invalidate_ColliderTransformationMatrix()
{
	/* Body Collider */
	{
		_matrix BoneMatrix = m_pTransformCom->Get_WorldMatrix();

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_WeaponWorldMatrix[TYPE_BODY], m_pTransformCom->Get_WorldMatrix());

		m_pColliderCom[TYPE_BODY]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[TYPE_BODY]));
	}


	/* Sword Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedBoneMatrixPtr("hand_L"));

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_WeaponWorldMatrix[TYPE_SWORD], BoneMatrix * m_pTransformCom->Get_WorldMatrix());

		m_pColliderCom[TYPE_SWORD]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[TYPE_SWORD]));
	}


	/* Shield Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedBoneMatrixPtr("hand_R"));

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_WeaponWorldMatrix[TYPE_SHIELD], BoneMatrix * m_pTransformCom->Get_WorldMatrix());

		m_pColliderCom[TYPE_SHIELD]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[TYPE_SHIELD]));
	}

	for (size_t i = 0; i < TYPE_END; i++)
	{
		m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[i]));
	}
}

HRESULT CBlueMoriblin::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BlueMoriblinSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVI_DESC NaviDesc{};
	{
		NaviDesc.iStartCellIndex = 0;
	}
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
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
			TEXT("Com_Monster_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_BODY]), &BodyBoundingDesc)))
			return E_FAIL;
	}	
	

	/* Sword Com_Collider */
	{
		CBounding_OBB::OBB_DESC		SwordBoundingDesc{};
		{
			SwordBoundingDesc.vExtents = _float3(0.2f, 0.1f, 0.7f);
			SwordBoundingDesc.vCenter = _float3(0.1f, SwordBoundingDesc.vExtents.y, -0.69f);
			SwordBoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sword_OBB"),
			TEXT("Com_Monster_Sword_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_SWORD]), &SwordBoundingDesc)))
			return E_FAIL;
	}

	/* Shield Com_Collider */
	{
		CBounding_OBB::OBB_DESC		ShieldBoundingDesc{};
		{
			ShieldBoundingDesc.vExtents = _float3(0.1f, 0.5f, 0.5f);
			ShieldBoundingDesc.vCenter = _float3(-0.4f, ShieldBoundingDesc.vExtents.y - 0.45f, 0.f);
			ShieldBoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Shield_OBB"),
			TEXT("Com_Monster_Shield_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[TYPE_SHIELD]), &ShieldBoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlueMoriblin::Add_PartObjects()
{
	return S_OK;
}

HRESULT CBlueMoriblin::Bind_ShaderResources()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	_float4 CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

void CBlueMoriblin::Rotate_Monster(_bool* _bState)
{
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vToPlayerLook = XMVector3Normalize(m_vDirToPlayer);

	_vector vPlayerLocalPos = XMVector3TransformCoord(m_vPlayerPos, m_pTransformCom->Get_WorldMatrix_Inverse());
	_vector vMonsterLocalPos = XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrix_Inverse());

	// 플레이어가 몬스터 기준 오른쪽에 있다 -> 오른쪽으로 돌다
	if (vPlayerLocalPos.m128_f32[0] >= vMonsterLocalPos.m128_f32[0])
	{
		m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(160.f));
	}

	// 플레이어가 몬스터 기준 왼쪽에 있다 -> 왼쪽으로 돌다
	else if (vPlayerLocalPos.m128_f32[0] < vMonsterLocalPos.m128_f32[0])
	{
		m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(-160.f));
	}

	// 방향을 계속 돌다가, 플레이어를 바라보는 방향벡터와 몬스터의 look 벡터가 비슷할 경우 -> 적 발견!!
	if (Compare_DirVectors(vLook, vToPlayerLook))
	{
		*_bState = true;
	}
}

void CBlueMoriblin::Collision_ToPlayer()
{
	// 플레이어
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
	CModel* pPlayerModel =dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
	
	// 몬스터 위치 
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	// 이팩트 위치
	CTransform* pEffect_Blood = dynamic_cast<CTransform*>(m_pEffect_Blood->Get_Component(TEXT("Com_Transform")));// 이팩트 위치
	CTransform* pEffect_Metal = dynamic_cast<CTransform*>(m_pEffect_Metal->Get_Component(TEXT("Com_Transform")));

	if (nullptr != pPlayerModel)
		iCurAnim = pPlayerModel->Get_CurAnimation();

	// 몬스터 칼 & 플레이어 몸
	if (Collision_ToPlayerBody(TYPE_SWORD)
		&& STATE_SHIELD != iCurAnim
		&& !m_bDead)
	{
		m_pGameInstance->PlaySoundEx("hit.wav", SOUNDTYPE::HIT, 0.4f);
		m_pGameInstance->PlaySoundEx("Moriblin_Vo_Attack00.wav", SOUNDTYPE::MON_ATK, 0.5f);
		// 여기서 플레이어 피타격 모션
		if (iCurAnim != -1)
		{
			pPlayerModel->Get_VecAnims()[iCurAnim]->Set_TrackPosition(0.f);
			pPlayerModel->Get_VecAnims()[STATE_DMG_QUAKE]->Set_TrackPosition(0.f);
		}

		pPlayer->Set_CurState(STATE_DMG_QUAKE);
		pPlayer->Set_CollMonster(CPlayer::BLUEMORIBLIN);
	}

	if (Collision_ToPlayerBody(TYPE_BODY))
	{	
		Handle_Spheres_Sliding(pPlayer);
		m_pGameInstance->PlaySoundEx("Link_Pull.wav", SOUNDTYPE::MON_PUSH, 0.4f);
	}

	// 몬스터 칼 & 플레이어 방패
	if (Collision_ToPlayerShield(TYPE_SWORD) 
		&& STATE_SHIELD == iCurAnim
		&& m_bAttack && !m_bDead)
	{
		// 여기서 튕기기
		m_bStagger = true;

		m_bFind   = false;
		m_bWalk   = false;
		m_bAttack = false;
		m_bGuard  = false;
		m_bHit    = false;
		m_bDead   = false;

		// 이펙트
		m_pEffect_Metal->Set_EffectOnce(true);

		_vector vMidPos = {};
		{
			vMidPos.m128_f32[0] = vMonsterPos.m128_f32[0] - 0.3f * (vMonsterPos.m128_f32[0] - m_vPlayerPos.m128_f32[0]);
			vMidPos.m128_f32[1] = vMonsterPos.m128_f32[1] - 0.3f * (vMonsterPos.m128_f32[1] - m_vPlayerPos.m128_f32[1]) + 0.7f;
			vMidPos.m128_f32[2] = vMonsterPos.m128_f32[2] - 0.3f * (vMonsterPos.m128_f32[2] - m_vPlayerPos.m128_f32[2]);
		}

		pEffect_Metal->Set_State(CTransform::STATE_POSITION, vMidPos);

		m_pGameInstance->PlaySoundEx("Moriblin_Vo_Damage01.wav", SOUNDTYPE::MON_DAMAGE, 1.f);
		m_pGameInstance->PlaySoundEx("guard.wav", SOUNDTYPE::MON_GUARD, 1.f);
	}

	// 몬스터 몸 & 플레이어 칼
	else if (Collision_ToPlayerSword(TYPE_BODY)
		&& (STATE_SLASH == iCurAnim
			|| STATE_SLASH_LP == iCurAnim
			|| STATE_NORMAL_SLASH_ED == iCurAnim) && !m_bDead)
	{
		if (m_bStagger)
		{
			m_bHit = true;

			// 이펙트
			m_pEffect_Blood->Set_EffectOnce(true);
			
			_vector vMidPos = {};
			{
				vMidPos.m128_f32[0] = vMonsterPos.m128_f32[0] - 0.2f * (vMonsterPos.m128_f32[0] - m_vPlayerPos.m128_f32[0]);
				vMidPos.m128_f32[1] = vMonsterPos.m128_f32[1] - 0.2f * (vMonsterPos.m128_f32[1] - m_vPlayerPos.m128_f32[1]) + 0.7f;
				vMidPos.m128_f32[2] = vMonsterPos.m128_f32[2] - 0.2f * (vMonsterPos.m128_f32[2] - m_vPlayerPos.m128_f32[2]);
			}

			pEffect_Blood->Set_State(CTransform::STATE_POSITION, vMidPos);
		}
		else 
			m_bGuard = true;

		m_bFind	   = false;
		m_bWalk	   = false;
		m_bAttack  = false;
		m_bGuard   = false;
		m_bStagger = false;
		m_bDead    = false;

		m_pGameInstance->PlaySoundEx("Moriblin_Vo_Damage00.wav", SOUNDTYPE::HIT, 1.f);
	}
	// 플레이어 칼 & 몬스터 쉴드
	else if (Collision_ToPlayerSword(TYPE_SHIELD) 
		&& !m_bHit && !m_bDead
		&& (STATE_SLASH == iCurAnim
			|| STATE_SLASH_LP == iCurAnim
			|| STATE_NORMAL_SLASH_ED == iCurAnim))
	{
		m_bGuard = true;

		m_bFind    = false;
		m_bWalk    = false;
		m_bAttack  = false;
		m_bHit     = false;
		m_bStagger = false;
		m_bDead    = false;

		// 이펙트
		m_pEffect_Metal->Set_EffectOnce(true);

		_vector vMidPos = {};
		{
			vMidPos.m128_f32[0] = vMonsterPos.m128_f32[0] - 0.5f * (vMonsterPos.m128_f32[0] - m_vPlayerPos.m128_f32[0]);
			vMidPos.m128_f32[1] = vMonsterPos.m128_f32[1] - 0.5f * (vMonsterPos.m128_f32[1] - m_vPlayerPos.m128_f32[1]) + 0.7f;
			vMidPos.m128_f32[2] = vMonsterPos.m128_f32[2] - 0.5f * (vMonsterPos.m128_f32[2] - m_vPlayerPos.m128_f32[2]);
		}

		pEffect_Metal->Set_State(CTransform::STATE_POSITION, vMidPos);

		m_pGameInstance->PlaySoundEx("guard.wav", SOUNDTYPE::MON_GUARD, 1.f);
	}


	if (m_pEffect_Blood->Get_EffectOnce())
	{
		m_pEffect_Blood->Animate_Texture(false);
	}

	if (m_pEffect_Metal->Get_EffectOnce())
	{
		m_pEffect_Metal->Animate_Texture(false);
	}

}

_bool CBlueMoriblin::Collision_ToPlayerBody(MCOLLIDER _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

_bool CBlueMoriblin::Collision_ToPlayerSword(MCOLLIDER _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));	
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Sword_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

_bool CBlueMoriblin::Collision_ToPlayerShield(MCOLLIDER _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Shield_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

void CBlueMoriblin::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
	///////////////// Player 중점과 반지금///////////////////////////////////////////////////
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

	CBounding_Sphere* pMonsterSphere = dynamic_cast<CBounding_Sphere*>(m_pColliderCom[TYPE_BODY]->Get_Bounding());
	if (nullptr == pMonsterSphere)
		return;

	_float vMonsterRadius = ((BoundingSphere*)(pMonsterSphere->Get_Bounding()))->Radius;
	_float3 vMonsterCenter = ((BoundingSphere*)(pMonsterSphere->Get_Bounding()))->Center;
	///////////////////////////////////////////////////////////////////////////////////////////////

	// 서로 바라보는두 방향벡터 구해줌
	_vector vDirToPlayer = XMVector3Normalize(XMLoadFloat3(&vPlayerCenter) - XMLoadFloat3(&vMonsterCenter));
	_vector vDirToMonster = XMVector3Normalize(XMLoadFloat3(&vMonsterCenter) - XMLoadFloat3(&vPlayerCenter));

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


CBlueMoriblin* CBlueMoriblin::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBlueMoriblin* pInstance = new CBlueMoriblin(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBlueMoriblin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlueMoriblin::Clone(void* _pArg, _uint _iLevel)
{
	CBlueMoriblin* pInstance = new CBlueMoriblin(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlueMoriblin::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Blood);
	Safe_Release(m_pEffect_Metal);

	for (size_t i = 0; i < TYPE_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
