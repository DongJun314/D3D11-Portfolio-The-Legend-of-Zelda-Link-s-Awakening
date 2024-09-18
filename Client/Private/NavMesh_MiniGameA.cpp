#include "stdafx.h"
#include "NavMesh_MiniGameA.h"

// Engine
#include "GameInstance.h"

CNavMesh_MiniGameA::CNavMesh_MiniGameA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CNavMesh_MiniGameA::CNavMesh_MiniGameA(const CNavMesh_MiniGameA& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CNavMesh_MiniGameA::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNavMesh_MiniGameA::Initialize(void* _pArg)
{
    //m_pNavigationCom.Get
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CNavMesh_MiniGameA::Priority_Tick(_float _fTimeDelta)
{
}

void CNavMesh_MiniGameA::Tick(_float _fTimeDelta)
{
    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CNavMesh_MiniGameA::Late_Tick(_float _fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CNavMesh_MiniGameA::Render()
{
#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CNavMesh_MiniGameA::Add_Component()
{
    /* Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

CNavMesh_MiniGameA* CNavMesh_MiniGameA::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavMesh_MiniGameA* pInstance = new CNavMesh_MiniGameA(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CNavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNavMesh_MiniGameA::Clone(void* _pArg, _uint _iLevel)
{
    CNavMesh_MiniGameA* pInstance = new CNavMesh_MiniGameA(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavMesh");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavMesh_MiniGameA::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
