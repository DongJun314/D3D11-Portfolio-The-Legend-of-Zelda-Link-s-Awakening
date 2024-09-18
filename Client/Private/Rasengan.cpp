#include "stdafx.h"
#include "Rasengan.h"

// Client 
#include "Player.h"
#include "PotDemon.h"
#include "PotDemonKing.h"

#include "Animation.h"

CRasengan::CRasengan(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPotDemonPartObject(_pDevice, _pContext)
{
}

CRasengan::CRasengan(const CRasengan& _rhs)
	: CPotDemonPartObject(_rhs)
{
}

HRESULT CRasengan::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRasengan::Initialize(void* _pArg)
{
	RASENGAN_DESC* pRasengan = (RASENGAN_DESC*)(_pArg);
	{
		m_pSocketMatrix = pRasengan->pSocketMatrix;
		m_bOnFireRasengan = pRasengan->bOnFireRasengan;
		m_iRasenganState = pRasengan->iRasenganState;
		m_iRasengan_CloneCount = pRasengan->iRasengan_CloneCount;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// Boss Monster
	_vector vecPos = { 1.58f, 10.5f, 59.7f, 1.0f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

	return S_OK;
}

void CRasengan::Priority_Tick(_float _fTimeDelta)
{
	//Invalidate_ColliderTransformationMatrix();
	//XMStoreFloat4x4(&m_WorldMatrix, m_pParentTransform->Get_WorldMatrix());
}

void CRasengan::Tick(_float _fTimeDelta)
{
	//m_fSetScaling += _fTimeDelta;

	m_fSetScaling += 0.1f;


	if ((*m_iRasenganState) == KING_ATK_ULT)
	{
		if (m_iRasengan_CloneCount == 0)
		{
			Invalidate_TransformationMatrix();
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

		/*	_float4 vTmp, vSrc, vDst;
			XMStoreFloat4(&vTmp, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[0]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[0], &vTmp, sizeof(_float4));

			XMStoreFloat4(&vSrc, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[1]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[1], &vSrc, sizeof(_float4));

			XMStoreFloat4(&vDst, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[2]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[2], &vDst, sizeof(_float4));*/

			//m_pTransformCom->Set_Scaling(m_fSetScaling, m_fSetScaling, m_fSetScaling);
		}
		else
		{
			if ((*m_bOnFireRasengan) == true)
			{
				Invalidate_TransformationMatrix();

				m_vMonsterPos = { m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43 };

				//+ 여기서 플레이어 위치 한번만 받아오기
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
				_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

				m_vDirToPlayer = XMVector3Normalize(vPlayerPos - m_vMonsterPos);
				m_vDirToMonster = XMVector3Normalize(m_vMonsterPos - vPlayerPos);

				(*m_bOnFireRasengan) = false;
				
			}
			if (m_WorldMatrix._42 < 9.f)
			{
				(*m_iRasenganState) = POTDEMON_STATE_END;
			}
			_float fSpeed = 0.5f;
			m_vMonsterPos += m_vDirToPlayer * fSpeed;

			//////////////////////////////////
			_float4 vTmp, vSrc, vDst;
			XMStoreFloat4(&vTmp, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[0]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[0], &vTmp, sizeof(_float4));

			XMStoreFloat4(&vSrc, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[1]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[1], &vSrc, sizeof(_float4));

			XMStoreFloat4(&vDst, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[2]) * m_fSetScaling);
			memcpy(&m_WorldMatrix.m[2], &vDst, sizeof(_float4));

			/////////////////////////////////

			_float4 fMonsterPos;
			XMStoreFloat4(&fMonsterPos, XMVectorSetW(m_vMonsterPos, 1.f));
			memcpy(&m_WorldMatrix.m[3], &fMonsterPos, sizeof(_float4));
			m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

		}
	}
}

void CRasengan::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();

	// Turn 함수는 동적회전
	_vector vY = { 0.f, 1.f, 0.f, 0.f };
	_matrix RotationMatrix = XMMatrixRotationAxis(vY, XMConvertToRadians(90.0f) * _fTimeDelta);

	for (size_t i = 0; i < 3; i++)
	{
		_vector vStateDir = XMLoadFloat4x4(&m_WorldMatrix).r[i];

		_float4 vTmp;
		XMStoreFloat4(&vTmp, XMVector4Transform(vStateDir, RotationMatrix));
		memcpy(&m_WorldMatrix.m[i], &vTmp, sizeof(_float4));
	}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CRasengan::Render()
{
	if ((*m_iRasenganState) == KING_ATK_ULT)
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
		//m_pColliderCom->Render();
#endif
	}

	return S_OK;
}

void CRasengan::Collision_ToPlayer()
{
	_int iCurAnim = { -1 };
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
	if (nullptr != pModel)
		iCurAnim = pModel->Get_CurAnimation();

	if (Collision_ToPlayerBody())
	{
		/*if (iCurAnim != -1)
		{
			pModel->Get_VecAnims()[iCurAnim]->Set_TrackPosition(0.f);
			pModel->Get_VecAnims()[STATE_DMG_QUAKE]->Set_TrackPosition(0.f);
		}*/

		pPlayer->Set_CollMonster(CPlayer::RANSENGAN);
		pPlayer->Set_CurState(STATE_DMG_QUAKE);
	}
}

_bool CRasengan::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CRasengan::Collision_ToPlayerSword()
{
	return _bool();
}

_bool CRasengan::Collision_ToPlayerShield()
{
	return _bool();
}

void CRasengan::Invalidate_TransformationMatrix()
{
	CPotDemon* pPopDemon = dynamic_cast<CPotDemon*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), 3));
	CPotDemonKing* pDemonKing = dynamic_cast<CPotDemonKing*>(pPopDemon->Get_PartObject(TEXT("Part_PotDemon_King")));

	_float4x4 WorldMatrix = pDemonKing->Get_FireC_WorldMatrix();

	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&WorldMatrix));
}

HRESULT CRasengan::Add_Component()
{
	/* Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}

	/* Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		Rasengan_BoundingDesc{};
		{
			Rasengan_BoundingDesc.fRadius = 10.f;
			Rasengan_BoundingDesc.vCenter = _float3(0.f, Rasengan_BoundingDesc.fRadius, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &Rasengan_BoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRasengan::Bind_ShaderResources()
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

void CRasengan::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}

CRasengan* CRasengan::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRasengan* pInstance = new CRasengan(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasengan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRasengan::Clone(void* _pArg, _uint _iLevel)
{
	CRasengan* pInstance = new CRasengan(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CRasengan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRasengan::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
