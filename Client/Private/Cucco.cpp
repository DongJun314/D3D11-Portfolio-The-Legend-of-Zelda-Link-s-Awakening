#include "stdafx.h"
#include "Cucco.h"

// Client 
#include "Player.h"
#include "PotDemon.h"
#include "PotDemonKing.h"

#include "Animation.h"

CCucco::CCucco(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPotDemonPartObject(_pDevice, _pContext)
{
}

CCucco::CCucco(const CCucco& _rhs)
	: CPotDemonPartObject(_rhs)
{
}

HRESULT CCucco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCucco::Initialize(void* _pArg)
{
	CUCCO_DESC* pCuccoDesc = (CUCCO_DESC*)(_pArg);
	{
		m_pSocketMatrix = pCuccoDesc->pSocketMatrix;
		m_bOnFireCucco = pCuccoDesc->bOnFireCucco;
		m_iCuccoState = pCuccoDesc->iCuccoState;
		m_iCucco_CloneCount = pCuccoDesc->iCucco_CloneCount;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pModelCom->Set_Init_Animation(0);
	
	return S_OK;
}

void CCucco::Priority_Tick(_float _fTimeDelta)
{
}

void CCucco::Tick(_float _fTimeDelta)
{
	if ((*m_iCuccoState) == KING_ATTACK_FIRE)
	{
		if (m_iCucco_CloneCount == 0)
		{
			Invalidate_TransformationMatrix();
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

			if (*m_iCuccoState == CS_LIFTED)
			{
				m_pModelCom->Set_Animation(0);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}

			if (*m_iCuccoState == CS_RUN)
			{
				m_pModelCom->Set_Animation(1);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}

			if (*m_iCuccoState == CS_WAIT)
			{
				m_pModelCom->Set_Animation(2);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}
		}
		else
		{
			if (!m_bGetInfoOnce)
			{
				Invalidate_TransformationMatrix();

				m_vMonsterPos = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };

				//+ 여기서 플레이어 위치 한번만 받아오기
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
				_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

				m_vDirToPlayer = XMVector3Normalize(vPlayerPos - m_vMonsterPos);

				m_bGetInfoOnce = true;
			}
			
			if (m_WorldMatrix._42 < 41.8f)
			{
				if (!m_bGetPosOnce)
				{
					m_pGameInstance->PlaySoundEx("BallGround.wav", SOUNDTYPE::MON_ATK, 0.5f);
					m_vMonsterPos = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };
					m_bGetPosOnce = true;
				}
			}
			else
			{
				_float fSpeed = 0.3f;
				m_vMonsterPos += m_vDirToPlayer * fSpeed;
			}
			
			_float4 fMonsterPos;
			XMStoreFloat4(&fMonsterPos, XMVectorSetW(m_vMonsterPos, 1.f));
			memcpy(&m_WorldMatrix.m[3], &fMonsterPos, sizeof(_float4));
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
		}


		m_pModelCom->Play_Animation(_fTimeDelta, true);
	}
}

void CCucco::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CCucco::Render()
{
	if ((*m_iCuccoState) == KING_ATTACK_FIRE)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			// 메시에게 영향을 주는 뼈들을 받아옴, 전체 뼈가 아닌,  뒤에 i는 그 몇번 메시 인지를 알기 위한 인자
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
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

void CCucco::Collision_ToPlayer()
{
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr != pModel)
		iCurAnim = pModel->Get_CurAnimation();

	if (Collision_ToPlayerBody())
	{
		pPlayer->Set_CurState(STATE_DMG_QUAKE);
		pPlayer->Set_CollMonster(CPlayer::CUCCO);

		/*if (iCurAnim != -1)
		{
			pModel->Get_VecAnims()[iCurAnim]->Set_TrackPosition(0.f);
			pModel->Get_VecAnims()[STATE_DMG_QUAKE]->Set_TrackPosition(0.f);
			pModel->Set_Animation(STATE_DMG_QUAKE);
			pPlayer->Set_CollMonster(CPlayer::CUCCO);
		}

		
		pPlayer->Set_CurState(STATE_DMG_QUAKE);*/
	}

	if (Collision_ToPlayerSword())
	{

	}

	if (Collision_ToPlayerShield())
	{

	}
}

_bool CCucco::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CCucco::Collision_ToPlayerSword()
{
	return _bool();
}

_bool CCucco::Collision_ToPlayerShield()
{
	return _bool();
}

void CCucco::Invalidate_TransformationMatrix()
{
	CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
	CPotDemonKing* pDemonKing = dynamic_cast<CPotDemonKing*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_King")));

	_float4x4 WorldMatrix = pDemonKing->Get_FireC_WorldMatrix();

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&WorldMatrix));

	Safe_AddRef(pPopDemon);
	Safe_AddRef(pDemonKing);
}

HRESULT CCucco::Add_Component()
{
	/* Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cucco"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}

	/* Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		Cucco_BoundingDesc{};
		{
			Cucco_BoundingDesc.fRadius = 0.5f;
			Cucco_BoundingDesc.vCenter = _float3(0.f, 0.5f , 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Cucco_BoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCucco::Bind_ShaderResources()
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

void CCucco::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}

CCucco* CCucco::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCucco* pInstance = new CCucco(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCucco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCucco::Clone(void* _pArg, _uint _iLevel)
{
	CCucco* pInstance = new CCucco(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCucco");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCucco::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
