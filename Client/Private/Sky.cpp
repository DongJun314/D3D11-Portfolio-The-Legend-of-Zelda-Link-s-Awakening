#include "stdafx.h"
#include "Sky.h"

#include "GameInstance.h"

CSky::CSky(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CSky::CSky(const CSky& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CSky::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSky::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
	{
		CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};
		{
			GameObjectDesc.iGameObjectData = 10;
			GameObjectDesc.fSpeedPerSec = 10.0f;
			GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);
		}		

		if (FAILED(__super::Initialize(&GameObjectDesc)))
			return E_FAIL;
	}

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CSky::Priority_Tick(_float _fTimeDelta)
{
}

void CSky::Tick(_float _fTimeDelta)
{
}

void CSky::Late_Tick(_float _fTimeDelta)
{
	_float4 vCamPos = m_pGameInstance->Get_CamPosition();
	_vector vecCamPos = XMLoadFloat4(&vCamPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecCamPos);
	//m_pTransformCom->Set_Scaling(500.f, 500.f, 500.f);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CSky::Render()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 5))) //6
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

    return S_OK;
}

HRESULT CSky::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

    return S_OK;
}

CSky* CSky::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSky* pInstance = new CSky(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}

    return pInstance;
}

CGameObject* CSky::Clone(void* _pArg, _uint _iLevel)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
