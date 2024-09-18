#include "stdafx.h"
#include "Mini_GameA_Back.h"

// Client
#include "Player.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CMini_GameA_Back::CMini_GameA_Back(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CMini_GameA_Back::CMini_GameA_Back(const CMini_GameA_Back& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CMini_GameA_Back::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMini_GameA_Back::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector vecPos = { 2.2f, 26.7f, 1993.5f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

    return S_OK;
}

void CMini_GameA_Back::Priority_Tick(_float _fTimeDelta)
{
}

void CMini_GameA_Back::Tick(_float _fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMini_GameA_Back::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CMini_GameA_Back::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

void CMini_GameA_Back::Collision_ToPlayer()
{
    if (Collision_ToPlayerBody())
    {
        // Set Next Scene
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
        if (nullptr == pPlayer)
            return;

        pPlayer->Set_MiniGameA(false);

        m_pGameInstance->StopSound(SOUNDTYPE::BGM);
        m_pGameInstance->PlayBGMLoop("1-08 Level 7 - Eagle's Tower.mp3", 0.3f);

        _vector Pos = { -3.6f, 12.7f, 84.4f, 1.0f };
        dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, Pos);
    }
}

_bool CMini_GameA_Back::Collision_ToPlayerBody()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return false;

    return m_pColliderCom->Intersect(pPlayerCollider);
}

HRESULT CMini_GameA_Back::Add_Component()
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

CMini_GameA_Back* CMini_GameA_Back::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMini_GameA_Back* pInstance = new CMini_GameA_Back(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMini_GameA_Back");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMini_GameA_Back::Clone(void* _pArg, _uint _iLevel)
{
    CMini_GameA_Back* pInstance = new CMini_GameA_Back(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMini_GameA_Back");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMini_GameA_Back::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
