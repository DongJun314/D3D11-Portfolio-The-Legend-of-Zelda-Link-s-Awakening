#include "Light_Manager.h"

// Engine
#include "Light.h"

CLight_Manager::CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& _LightDesc)
{
	CLight* pLight = CLight::Create(m_pDevice, m_pContext, _LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_listLights.push_back(pLight);

	return S_OK;
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint _iIndex) const
{
	auto iter = m_listLights.begin();

	for (size_t i = 0; i < _iIndex; i++)
		++iter;

	return (*iter)->Get_LightDesc();
}

CLight_Manager* CLight_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLight_Manager* pInstance = new CLight_Manager(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLight_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_listLights)
		Safe_Release(pLight);

	m_listLights.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
