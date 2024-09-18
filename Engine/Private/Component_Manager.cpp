#include "Component_Manager.h"
#include "Component.h"

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Initialize(_uint _iNumLevels)
{
	m_iNumLevels = _iNumLevels;

	m_pMapPrototypes = new PROTOTYPES[_iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag, class CComponent* _pPrototype)
{
	if (nullptr != Find_Prototype(_iLevelIndex, _strPrototypeTag))
		return E_FAIL;

	m_pMapPrototypes[_iLevelIndex].emplace(_strPrototypeTag, _pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, void* _pArg)
{
	CComponent* pPrototype = Find_Prototype(_iLevelIndex, _strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(_pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Clear(_uint _iLevelIndex)
{
	for (auto& Pair : m_pMapPrototypes[_iLevelIndex])
		Safe_Release(Pair.second);

	m_pMapPrototypes[_iLevelIndex].clear();
}

CComponent* CComponent_Manager::Find_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag)
{
	if (_iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = m_pMapPrototypes[_iLevelIndex].find(_strPrototypeTag);
	if (iter == m_pMapPrototypes[_iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CComponent_Manager* CComponent_Manager::Create(_uint _iNumLevels)
{
	CComponent_Manager* pInstance = new CComponent_Manager();

	if (FAILED(pInstance->Initialize(_iNumLevels)))
	{
		MSG_BOX("Failed to Created : Component_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComponent_Manager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pMapPrototypes[i])
		{
			Safe_Release(Pair.second);
		}
		m_pMapPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pMapPrototypes);
}

