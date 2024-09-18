#include "Level.h"

// Engine
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    ,m_pContext(_pContext)
    ,m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel::Initialize()
{
    return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
    return S_OK;
}

void CLevel::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
