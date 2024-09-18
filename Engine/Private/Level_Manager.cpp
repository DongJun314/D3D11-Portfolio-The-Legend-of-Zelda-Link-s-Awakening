#include "Level_Manager.h"

// Engine
#include "Level.h"
#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Initialize()
{
	return S_OK;
}

void CLevel_Manager::Tick(float _fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(_fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Open_Level(_uint _iLevelIndex, CLevel* _pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
		m_pGameInstance->Clear(m_iLevelIndex);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = _pNewLevel;

	m_iLevelIndex = _iLevelIndex;
	
	return S_OK;
}

CLevel_Manager* CLevel_Manager::Create()
{
	CLevel_Manager* pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}
