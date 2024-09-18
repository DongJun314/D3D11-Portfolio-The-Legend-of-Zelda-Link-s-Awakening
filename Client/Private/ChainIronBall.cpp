#include "stdafx.h"
#include "ChainIronBall.h"

// Client 
#include "Player.h"
#include "PotDemon.h"
#include "PotDemonKing.h"

#include "Animation.h"

CChainIronBall::CChainIronBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPotDemonPartObject(_pDevice, _pContext)
{
}

CChainIronBall::CChainIronBall(const CChainIronBall& _rhs)
	: CPotDemonPartObject(_rhs)
{
}

HRESULT CChainIronBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChainIronBall::Initialize(void* _pArg)
{
	CHAINIRONBALL_DESC* pChainIronBall = (CHAINIRONBALL_DESC*)(_pArg);
	{
		m_pSocketMatrix = pChainIronBall->pSocketMatrix;
		m_bOnFireChainIronBall = pChainIronBall->bOnFireChainIronBall;
		m_iChainIronBallState = pChainIronBall->iChainIronBallState;
		m_iChainIronBall_CloneCount = pChainIronBall->iChainIronBall_CloneCount;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CChainIronBall::Priority_Tick(_float _fTimeDelta)
{
}

void CChainIronBall::Tick(_float _fTimeDelta)
{
	if ((*m_iChainIronBallState) == KING_ATTACK_FIRE)
	{
		if (m_iChainIronBall_CloneCount == 0)
		{
			// 이 함수에서 FireBall C 의 matrix정보를 받아옵니다
			Invalidate_TransformationMatrix();
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
		}
		
		if (m_iChainIronBall_CloneCount > 0)
		{
			if (!m_bGetInfoOnce)
			{
				Invalidate_TransformationMatrix();
				
				m_vMonsterPos = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };
				
				//+ 여기서 플레이어 위치 한번만 받아오기
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
				_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

				m_vDirToPlayer = XMVector3Normalize(vPlayerPos - m_vMonsterPos);
				m_vDirToMonster = XMVector3Normalize(m_vMonsterPos - vPlayerPos);

				m_bGetInfoOnce = true;
			}

			if (m_WorldMatrix._42 < 41.8f)
			{
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->StopSound(SOUNDTYPE::MON_GUARD);
					m_pGameInstance->PlaySoundEx("BallGround.wav", SOUNDTYPE::MON_GUARD, 0.5f);
					m_vMonsterPos = { m_WorldMatrix._41, m_WorldMatrix._42,m_WorldMatrix._43 };
					m_bGetPosOnce = true;
				}
			}
			else
			{
				_float fSpeed = 0.5f;
				m_vMonsterPos += m_vDirToPlayer * fSpeed;
			}

			_float4 fMonsterPos;
			XMStoreFloat4(&fMonsterPos, XMVectorSetW(m_vMonsterPos, 1.f));
			memcpy(&m_WorldMatrix.m[3], &fMonsterPos, sizeof(_float4));
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
		}
	}
}

void CChainIronBall::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CChainIronBall::Render()
{
	if ((*m_iChainIronBallState) == KING_ATTACK_FIRE)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}

#ifdef _DEBUG
		m_pColliderCom->Render();
#endif
	}

	return S_OK;
}

void CChainIronBall::Collision_ToPlayer()
{
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr != pModel)
		iCurAnim = pModel->Get_CurAnimation();

	if (Collision_ToPlayerBody())
	{
		pPlayer->Set_CollMonster(CPlayer::CHAINIRONBALL);
		pPlayer->Set_CurState(STATE_DMG_QUAKE);

		/*pPlayer->Set_CollMonster(CPlayer::CHAINIRONBALL);

		if (iCurAnim != -1)
		{
			pModel->Get_VecAnims()[iCurAnim]->Set_TrackPosition(0.f);
			pModel->Get_VecAnims()[STATE_DMG_QUAKE]->Set_TrackPosition(0.f);
			pModel->Set_Animation(STATE_DMG_QUAKE);
			pPlayer->Set_CurState(STATE_DMG_QUAKE);
		}*/
	}
}

_bool CChainIronBall::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CChainIronBall::Collision_ToPlayerSword()
{
	return _bool();
}

_bool CChainIronBall::Collision_ToPlayerShield()
{
	return _bool();
}

void CChainIronBall::Invalidate_TransformationMatrix()
{
	CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
	CPotDemonKing* pDemonKing = dynamic_cast<CPotDemonKing*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_King")));

	_float4x4 WorldMatrix = pDemonKing->Get_FireB_WorldMatrix();

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&WorldMatrix));

	Safe_AddRef(pPopDemon);
	Safe_AddRef(pDemonKing);
}

HRESULT CChainIronBall::Add_Component()
{
	/* Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChainIronBall"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}

	/* Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		ChainIronBall_BoundingDesc{};
		{
			ChainIronBall_BoundingDesc.fRadius = 0.5f;
			ChainIronBall_BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ChainIronBall_BoundingDesc)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CChainIronBall::Bind_ShaderResources()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	// 조명 관련 쉐이더에 던지기
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

	// 받아온 카메라 위치 정보 쉐이더에 던지기
	_float4 CamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CChainIronBall::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}

CChainIronBall* CChainIronBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CChainIronBall* pInstance = new CChainIronBall(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CChainIronBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChainIronBall::Clone(void* _pArg, _uint _iLevel)
{
	CChainIronBall* pInstance = new CChainIronBall(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CChainIronBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChainIronBall::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
