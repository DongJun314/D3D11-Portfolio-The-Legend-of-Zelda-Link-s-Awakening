#include "stdafx.h"
#include "NavMesh_BossMap.h"

// Engine
#include "GameInstance.h"

CNavMesh_BossMap::CNavMesh_BossMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CNavMesh_BossMap::CNavMesh_BossMap(const CNavMesh_BossMap& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CNavMesh_BossMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CNavMesh_BossMap::Initialize(void* _pArg)
{
    //m_pNavigationCom.Get
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

void CNavMesh_BossMap::Priority_Tick(_float _fTimeDelta)
{
}

void CNavMesh_BossMap::Tick(_float _fTimeDelta)
{
    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CNavMesh_BossMap::Late_Tick(_float _fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CNavMesh_BossMap::Render()
{
#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CNavMesh_BossMap::Add_Component()
{
    /* Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

CNavMesh_BossMap* CNavMesh_BossMap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavMesh_BossMap* pInstance = new CNavMesh_BossMap(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CNavMesh_BossMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CNavMesh_BossMap::Clone(void* _pArg, _uint _iLevel)
{
    CNavMesh_BossMap* pInstance = new CNavMesh_BossMap(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavMesh_BossMap");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavMesh_BossMap::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
}
