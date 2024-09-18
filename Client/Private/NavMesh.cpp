#include "stdafx.h"
#include "NavMesh.h"

// Engine
#include "GameInstance.h"

CNavMesh::CNavMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CNavMesh::CNavMesh(const CNavMesh& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CNavMesh::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNavMesh::Initialize(void* _pArg)
{
    //m_pNavigationCom.Get
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CNavMesh::Priority_Tick(_float _fTimeDelta)
{
}

void CNavMesh::Tick(_float _fTimeDelta)
{
    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
    m_pGameA_NavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
    m_pBossMap_NavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CNavMesh::Late_Tick(_float _fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CNavMesh::Render()
{
#ifdef _DEBUG
    m_pNavigationCom->Render();
    m_pGameA_NavigationCom->Render();
    m_pBossMap_NavigationCom->Render();
#endif
    
    return S_OK;
}

HRESULT CNavMesh::Add_Component()
{
    /* Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_MiniGameA"),
        TEXT("Com_Navigation_MiniGameA"), reinterpret_cast<CComponent**>(&m_pGameA_NavigationCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Boss_Map"),
        TEXT("Com_Navigation_Boss_Map"), reinterpret_cast<CComponent**>(&m_pBossMap_NavigationCom))))
        return E_FAIL;

    return S_OK;
}

CNavMesh* CNavMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavMesh* pInstance = new CNavMesh(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CNavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNavMesh::Clone(void* _pArg, _uint _iLevel)
{
    CNavMesh* pInstance = new CNavMesh(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavMesh::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
