#include "stdafx.h"
#include "Collider_BossToMain.h"

// Client
#include "Player.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CCollider_BossToMain::CCollider_BossToMain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CCollider_BossToMain::CCollider_BossToMain(const CCollider_BossToMain& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CCollider_BossToMain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCollider_BossToMain::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector vecPos = { 3000.f, 60.f, -200.f, 1.0f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

    return S_OK;
}

void CCollider_BossToMain::Priority_Tick(_float _fTimeDelta)
{
}

void CCollider_BossToMain::Tick(_float _fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CCollider_BossToMain::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CCollider_BossToMain::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

void CCollider_BossToMain::Collision_ToPlayer()
{
    if (Collision_ToPlayerBody())
    {
        // Set Next Scene
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
        if (nullptr == pPlayer)
            return;

        pPlayer->Set_IsBossStage(false);

        _vector vPos = { -3.6f, 12.7f, 84.4f, 1.0f };
        dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPos);
    }
}

_bool CCollider_BossToMain::Collision_ToPlayerBody()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return false;

    return m_pColliderCom->Intersect(pPlayerCollider);
}

HRESULT CCollider_BossToMain::Add_Component()
{
    /* Scene Com_Collider */
    {
        CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
        {
            BodyBoundingDesc.fRadius = 0.5f;
            BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius, 0.f);
        }

        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CCollider_BossToMain* CCollider_BossToMain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCollider_BossToMain* pInstance = new CCollider_BossToMain(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCollider_BossToMain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCollider_BossToMain::Clone(void* _pArg, _uint _iLevel)
{
    CCollider_BossToMain* pInstance = new CCollider_BossToMain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider_BossToMain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_BossToMain::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}