#include "stdafx.h"
#include "Zombie.h"

#include "Player.h"
#include "Animation.h"

#include "Effect_Texture.h"

CZombie::CZombie(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CZombie::CZombie(const CZombie& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CZombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CZombie::Initialize(void* _pArg)
{
	LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)_pArg;
	{
		pGameObjectDesc->fSpeedPerSec = 1.f;
		pGameObjectDesc->fRotationPerSec = XMConvertToRadians(180.f);
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_matrix WorldMatrix = XMMatrixSet
	  (-1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.f, 0.0f, -1.0f, 0.0f,
		-29.32f, 7.6f, 35.1f, 1.0f);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	m_pModelCom->Set_Init_Animation(0);

	m_fStagger = 20.f;

	m_pEffect_Blood = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect")));
	if (nullptr == m_pEffect_Blood)
		return E_FAIL;

	Safe_AddRef(m_pEffect_Blood);

	return S_OK;
}

void CZombie::Priority_Tick(_float _fTimeDelta)
{
	Invalidate_ColliderTransformationMatrix();
}

void CZombie::Tick(_float _fTimeDelta)
{
	// 플레이어의 위치 정보를 받아와서 카메라 위치로 세팅해주기
	{
		CGameObject* pGameObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		m_vPlayerPos = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

		m_vDirToPlayer = m_vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_fDisToPlayer = XMVector3Length(m_vDirToPlayer).m128_f32[0];
	}

	// 거리안에 있다?  발견
	if (m_fDisToPlayer <= 4.f && !m_bDead)
	{
		if (!m_bFindBefore)
			m_bFind = true;
		else
			m_bAttack= true;
	}
	else if (!m_bDead && !m_bFind) // 반복 재생
	{
		m_bAvoidSuddenTurn = false;
		m_bFind = false;
		m_bWalk = false;
		m_bAttack = false;
		m_bGuard = false;
		m_bHit = false;
		m_bStagger = false;
		m_bWait = false;
		m_bDead = false;
		

		m_fStagger = 20.f;
		m_fTimeDelta += 0.01f;

		if (m_fTimeDelta >= 3.f)
		{
			_vector vDepopPos = { -29.32f, m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1], 35.1f, 1.f};
			_vector vDepopLook = vDepopPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fDisToDepopPos = XMVectorGetX(XMVector3Length(vDepopLook));

			if (!m_bDepop)
			{
				m_pTransformCom->LookAt(vDepopPos);

				m_pTransformCom->Set_SpeedPerSec(0.5f);
				m_pTransformCom->Go_Straight(_fTimeDelta);

				m_pModelCom->Set_Animation(3);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
			} 
			
			if (fDisToDepopPos <= 1.f)
			{
				m_bDepop = true;
			}
			else
				m_bDepop = false;
		}
		else if(!m_bDepop && m_bFindBefore)
		{
			m_pModelCom->Set_Animation(2);
			m_pModelCom->Play_Animation(_fTimeDelta, true);
		}
	}


	if (m_bFind && !m_bWalk && !m_bAttack && !m_bGuard && !m_bHit && !m_bStagger && !m_bDead)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[1]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[1]->Get_TrackPosition();

		m_pModelCom->Set_Animation(1);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >= 36 && dTrackPosition <= 52)
		{
			m_pTransformCom->Set_SpeedPerSec(1.29f); 
			m_pTransformCom->Go_Up(_fTimeDelta);
		}
		if (dTrackPosition >= 56 && dTrackPosition < 72)
		{
			m_pTransformCom->Set_SpeedPerSec(1.5f);
			m_pTransformCom->Go_Up(_fTimeDelta);

			m_pTransformCom->Set_SpeedPerSec(1.0f);
			m_pTransformCom->Go_Straight(_fTimeDelta);
		}
		if (dTrackPosition >= 72)
		{
			m_bFind = false;
			m_bAttack = true;
			m_bFindBefore = true;
			m_bAvoidSuddenTurn = false;
		}

		/*if (!m_bSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("Zombie_Breath.mp3", SOUNDTYPE::MON_WALK, 1.f);
			m_bSoundOnce = true;
		}*/
	}
		
	if (m_bWalk && !m_bWait && !m_bDead)
	{
		m_pTransformCom->LookAt(m_vPlayerPos);
		m_pTransformCom->MoveTo(m_vPlayerPos, 2.f, _fTimeDelta);

		m_pModelCom->Set_Animation(3);
		m_pModelCom->Play_Animation(_fTimeDelta, true);

		if (m_fDisToPlayer < 3.f)
		{
			m_bWalk = false;
			m_bAttack = true;
		}
	}
	else if (m_bAttack && !m_bWait && !m_bDead)
	{
		m_bDepop = false;
		m_pModelCom->Set_Animation(3);
		m_pModelCom->Play_Animation(_fTimeDelta, true);

		//// 걸으면서 플레이어 위치에 따라서 방향을 바꿔주기
		if (m_bAvoidSuddenTurn)
		{
			// Look 방향으로 10.f 씩 쭉 이동하기, 중간 제지 불가 
			m_fStagger -= 0.5f;

			if (m_fStagger <= 0.f)
			{
				m_fStagger = 20.f;

				m_bWait = true;
				m_bAttack = false;
				m_bAvoidSuddenTurn = false;
			}
		
			m_pTransformCom->Set_SpeedPerSec(m_fStagger);
			m_pTransformCom->Go_Straight(_fTimeDelta/*, m_pNavigationCom*/);

			if (!m_bSoundOnce)
			{
				m_pGameInstance->PlaySoundEx("Zombie_Attack1.mp3", SOUNDTYPE::MON_ATK, 1.f);
				m_bSoundOnce = true;
			}
		}
		else
		{
			Rotate_Monster(&m_bAvoidSuddenTurn);

			_vector vAxisY = { 0.f, 1.f, 0.f, 0.f };
			m_pTransformCom->Turn(vAxisY, _fTimeDelta);
		}
	}
	else if (m_bWait && !m_bHit)
	{
		m_fTimeDelta += 0.01f;

		m_pModelCom->Set_Animation(2);
		m_pModelCom->Play_Animation(_fTimeDelta, true);

		if (m_fTimeDelta >= 2.f)
		{
			m_fTimeDelta = 0.f;

			m_bAttack = true;
			m_bWait = false;
		}

		/*if (!m_bSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("Zombie_Breath.mp3", SOUNDTYPE::MON_WALK, 1.f);
			m_bSoundOnce = true;
		}*/
	}
	else if (m_bHit)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[4]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[4]->Get_TrackPosition();

		m_pModelCom->Set_Animation(4);
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
		
		if (!m_bSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("Zombie_Attack2.mp3", SOUNDTYPE::MON_DAMAGE, 1.f);
		}
	}
	else if (m_bDepop)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[5]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[5]->Get_TrackPosition();

		m_pModelCom->Set_Animation(5);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (dTrackPosition >72 && dTrackPosition <= 86)
		{
			m_pTransformCom->Set_SpeedPerSec(1.5f);
			m_pTransformCom->Go_Straight(_fTimeDelta);

			m_pTransformCom->Set_SpeedPerSec(0.5f);
			m_pTransformCom->Go_Down(_fTimeDelta);
		}
		if (dTrackPosition > 86 && dTrackPosition < dDuration)
		{
			m_pTransformCom->Set_SpeedPerSec(0.5f);
			m_pTransformCom->Go_Down(_fTimeDelta);
		}
		if (dTrackPosition > dDuration)
		{
			m_fTimeDelta = 0.f;

			m_bDepop = false; 
			m_bFindBefore = false;
			
			_matrix WorldMatrix = XMMatrixSet
			(-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.f, 0.0f, -1.0f, 0.0f,
				-29.32f, 7.6f, 35.1f, 1.0f);

			m_pTransformCom->Set_WorldMatrix(WorldMatrix);
		}

		/*if (!m_bSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("Zombie_Breath.mp3", SOUNDTYPE::MON_GUARD, 1.f);
			m_bSoundOnce = true;
		}*/
	}
	else if (m_bDead)
	{
		m_bAttack = false;

		_double dDuration = m_pModelCom->Get_VecAnims()[6]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[6]->Get_TrackPosition();

		m_pModelCom->Set_Animation(6);
		m_pModelCom->Play_Animation(_fTimeDelta, false);

		if (m_bSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("3_Monster_Tail_Dead.wav", SOUNDTYPE::MON_DEAD, 1.f);
		}
	}

	m_bSoundOnce = false;
}

void CZombie::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	if(m_bAttack || m_bDead)
		SetUp_OnTerrain(m_pTransformCom, m_pNavigationCom);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CZombie::Render()
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
	for (size_t i = 0; i < ZTYPE_END; i++)
	{
		m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

void CZombie::Collision_ToPlayer()
{
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr != pModel)
		iCurAnim = pModel->Get_CurAnimation();

	// 몬스터 위치 
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 이팩트 위치
	CTransform* pEffect_Blood = dynamic_cast<CTransform*>(m_pEffect_Blood->Get_Component(TEXT("Com_Transform")));// 이팩트 위치

	// 몬스터 칼 & 플레이어 몸
	if (Collision_ToPlayerBody(ZTYPE_BODY)
		&& STATE_SHIELD != iCurAnim
		&& !m_bDead)
	{
		// 여기서 플레이어 피타격 모션
		pPlayer->Set_CurState(STATE_DMG_QUAKE);
		pPlayer->Set_CollMonster(CPlayer::ZOMBIE);
	}

	if (Collision_ToPlayerBody(ZTYPE_BODY)
		&& m_bDead)
	{
		Handle_Spheres_Sliding(pPlayer);

		if(m_bAttack)
			pPlayer->Set_CurState(STATE_DMG_QUAKE);
	}

	//// 몬스터 몸 & 플레이어 방패
	if (Collision_ToPlayerShield(ZTYPE_BODY)
		&& STATE_SHIELD == iCurAnim
		&& m_bAttack && !m_bDead)
	{
		// 여기서 튕기기
		m_bStagger = true;

		m_bFind = false;
		m_bWalk = false;
		m_bAttack = false;
		m_bGuard = false;
		m_bHit = false;
		m_bDead = false;
	}
	//// 몬스터 뒤 약점 & 플레이어 칼
	if (Collision_ToPlayerSword(ZTYPE_BACK)
		&& (STATE_SLASH == iCurAnim
			|| STATE_SLASH_LP == iCurAnim
			|| STATE_NORMAL_SLASH_ED == iCurAnim) 
		&& !m_bDead)
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

	if (m_pEffect_Blood->Get_EffectOnce())
	{
		m_pEffect_Blood->Animate_Texture(false);
	}
}

_bool CZombie::Collision_ToPlayerBody(ZOMBIE _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

_bool CZombie::Collision_ToPlayerSword(ZOMBIE _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Sword_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

_bool CZombie::Collision_ToPlayerShield(ZOMBIE _eType)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Shield_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom[_eType]->Intersect(pPlayerCollider);
}

void CZombie::Invalidate_ColliderTransformationMatrix()
{
	/* Body Collider */
	{
		_matrix BoneMatrix = m_pTransformCom->Get_WorldMatrix();

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_ColliderWorldMatrix[ZTYPE_BODY], m_pTransformCom->Get_WorldMatrix());

		m_pColliderCom[ZTYPE_BODY]->Tick(XMLoadFloat4x4(&m_ColliderWorldMatrix[ZTYPE_BODY]));
	}

	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pModelCom->Get_CombinedBoneMatrixPtr("hair_back_a3"));

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_ColliderWorldMatrix[ZTYPE_BACK], BoneMatrix * m_pTransformCom->Get_WorldMatrix());

		m_pColliderCom[ZTYPE_BACK]->Tick(XMLoadFloat4x4(&m_ColliderWorldMatrix[ZTYPE_BACK]));
	}

	for (size_t i = 0; i < ZTYPE_END; i++)
	{
		m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_ColliderWorldMatrix[i]));
	}
}

HRESULT CZombie::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zombie"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Navigation */
	CNavigation::NAVI_DESC NaviDesc{};
	{
		NaviDesc.iStartCellIndex = 153;
	}
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;


	/* Body Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
		{
			BodyBoundingDesc.fRadius = 0.7f;
			BodyBoundingDesc.vCenter = _float3(0.f,  BodyBoundingDesc.fRadius, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Monster_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[ZTYPE_BODY]), &BodyBoundingDesc)))
			return E_FAIL;
	}


	/* Back Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		BackBoundingDesc{};
		{
			BackBoundingDesc.fRadius = 0.4f;
			BackBoundingDesc.vCenter = _float3(0.f, -BackBoundingDesc.fRadius, -0.5f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Monster_Back_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[ZTYPE_BACK]), &BackBoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CZombie::Add_PartObjects()
{
	return S_OK;
}

HRESULT CZombie::Bind_ShaderResources()
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

void CZombie::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{///////////////// Player 중점과 반지금///////////////////////////////////////////////////
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
	if(!m_bDead)
	{
		_vector vPlayerDir = vDirToPlayer * (fOverLap);
		_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		vPlayerPos += vPlayerDir;
		dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPlayerPos);
	}
	else if(m_bDead)
	{
		// 몬스터 밀려나기
		_vector vPlayerDir = vDirToPlayer * (fOverLap * 0.5f);
		_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		vPlayerPos += vPlayerDir;
		dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPlayerPos);

		_vector vMonsterDir = vDirToMonster * (fOverLap * 0.5f);
		_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vMonsterPos += vMonsterDir;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
	}
	
}

CZombie* CZombie::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CZombie* pInstance = new CZombie(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CZombie");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CZombie::Clone(void* _pArg, _uint _iLevel)
{
	CZombie* pInstance = new CZombie(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CZombie");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZombie::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Blood);

	for (size_t i = 0; i < ZTYPE_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
