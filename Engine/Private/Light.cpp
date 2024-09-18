#include "Light.h"

CLight::CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHT_DESC& _LightDesc)
{
	m_LightDesc = _LightDesc;

	return S_OK;
}

CLight* CLight::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHT_DESC& _LightDesc)
{
	CLight* pInstance = new CLight(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_LightDesc)))
	{
		MSG_BOX("Failed to Created : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
