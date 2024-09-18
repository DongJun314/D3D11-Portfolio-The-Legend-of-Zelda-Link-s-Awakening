#include "stdafx.h"
#include "Weapon.h"

#include <cmath>

CWeapon::CWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CWeapon::CWeapon(const CWeapon& _rhs)
	: CPartObject(_rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void* _pArg)
{
	WEAPON_DESC* pWeaponDesc = (WEAPON_DESC*)_pArg;
	{
		m_pSocketMatrix[WTYPE_SWORD] = pWeaponDesc->pSocketMatrix[WTYPE_SWORD];
		m_pSocketMatrix[WTYPE_SHIELD] = pWeaponDesc->pSocketMatrix[WTYPE_SHIELD];
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// 추후 무기를 교체할 경우
	{
		//m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);
		//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.5f, 0.f, 0.f, 1.f));	
	}
	
	return S_OK;
}

void CWeapon::Priority_Tick(_float _fTimeDelta)
{
}

void CWeapon::Tick(_float _fTimeDelta)
{
}

void CWeapon::Late_Tick(_float _fTimeDelta)
{
	/* Sword Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[WTYPE_SWORD]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_WeaponWorldMatrix[WTYPE_SWORD], m_pTransformCom->Get_WorldMatrix() * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

		m_pColliderCom[WTYPE_SWORD]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[WTYPE_SWORD]));
	}

	/* Shield Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[WTYPE_SHIELD]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_WeaponWorldMatrix[WTYPE_SHIELD], m_pTransformCom->Get_WorldMatrix() * BoneMatrix * m_pParentTransform->Get_WorldMatrix());

		m_pColliderCom[WTYPE_SHIELD]->Tick(XMLoadFloat4x4(&m_WeaponWorldMatrix[WTYPE_SHIELD]));
	}
	

	//for (size_t i = 0; i < WTYPE_END; i++)
	//{
	//	m_pColliderCom[i]->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	//}

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CWeapon::Render()
{
	// 추후에 무기를 교체할 경우
	{
		//if (FAILED(Bind_ShaderResources()))
		//	return E_FAIL;

		//_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

		//for (_uint i = 0; i < iNumMeshes; i++)
		//{
		//	if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
		//		return E_FAIL;

		//	if (FAILED(m_pShaderCom->Begin(0)))
		//		return E_FAIL;

		//	if (FAILED(m_pModelCom->Render(i)))
		//		return E_FAIL;
		//}

	}
	
#ifdef _DEBUG
	for (size_t i = 0; i < WTYPE_END; i++)
	{
		m_pColliderCom[i]->Render();
	}
#endif

	return S_OK;
}

HRESULT CWeapon::Add_Component()
{	
	// 추후에 무기를 교체할 경우
	{
		///* Com_Shader */
		//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		//	return E_FAIL;

		///* Com_Model */
		//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Link"),
		//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		//	return E_FAIL;
	}
	
	/* Sword Com_Collider */
	{
		CBounding_OBB::OBB_DESC		SwordBoundingDesc{};
		{
			SwordBoundingDesc.vExtents = _float3(0.15f, 0.1f, 0.4f);
			SwordBoundingDesc.vCenter  = _float3(0.1f, 0.f, -0.5f);
			SwordBoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sword_OBB"),
			TEXT("Com_Sword_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[WTYPE_SWORD]), &SwordBoundingDesc)))
			return E_FAIL;
	}
	

	/* Shield Com_Collider */
	{
		CBounding_OBB::OBB_DESC		ShieldBoundingDesc{};
		{
			ShieldBoundingDesc.vExtents = _float3(0.45f, 0.1f, 0.5f);
			ShieldBoundingDesc.vCenter  = _float3(0.f, -0.18f, 0.15f);
			ShieldBoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Shield_OBB"),
			TEXT("Com_Shield_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[WTYPE_SHIELD]), &ShieldBoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	_float4 CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

CWeapon* CWeapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWeapon* pInstance = new CWeapon(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void* _pArg, _uint _iLevel)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	//for (size_t i = 0; i < WTYPE_END; i++)
	//{
	//	Safe_Release(m_pColliderCom[i]);
	//}
	//Safe_Release(m_pShaderCom);
	//Safe_Release(m_pModelCom);
}
