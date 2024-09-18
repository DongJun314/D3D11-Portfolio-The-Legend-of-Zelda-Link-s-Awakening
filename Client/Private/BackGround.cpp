#include "stdafx.h"
#include "BackGround.h"

// Engine
#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CBackGround::CBackGround(const CBackGround& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* _pArg, _uint _iLevel)
{
	if (nullptr == _pArg)
	{
		CGameObject::GAMEOBJECT_DESC GameObjectDesc;
		ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));
		{
			GameObjectDesc.iGameObjectData = 10;
			GameObjectDesc.fSpeedPerSec = 10.f;
			GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.f);
		}

		if (FAILED(__super::Initialize(&GameObjectDesc)))
			return E_FAIL;
	}
	else
	{
		BACKGROUND_DESC* pBackDesc = (BACKGROUND_DESC*)_pArg;
		{
			m_iBackData = pBackDesc->iBackData;
		}

		if (FAILED(__super::Initialize(_pArg)))
			return E_FAIL;
	}

	if (FAILED(Add_Component(_iLevel)))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX * 0.7f;
	m_fSizeY = g_iWinSizeY * 0.7f;
	m_fX = g_iWinSizeX * 0.5f;
	m_fY = g_iWinSizeY * 0.5f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fSizeX, m_fSizeY, 0.f, 100.f));

	m_pTransformCom->Set_Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	return S_OK;
}

void CBackGround::Priority_Tick(_float _fTimeDelta)
{
	int a = 10;
}

void CBackGround::Tick(_float _fTimeDelta)
{
	int a = 10;
}

void CBackGround::Late_Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this)))
		return;
}

HRESULT CBackGround::Render()
{
	_float4x4	 ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixIdentity());

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Add_Component(_uint _iLevel)
{
	switch (_iLevel)
	{
	case LEVEL_LOADING:
		if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Loading"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;

	case LEVEL_LOGO:
		if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;

	case LEVEL_END:
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBackGround* pInstance = new CBackGround(_pDevice, _pContext);

 	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* _pArg, _uint _iLevel)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(_pArg, _iLevel)))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	//Safe_Release(m_pLogoTextureCom);
	//Safe_Release(m_pLoadingTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
