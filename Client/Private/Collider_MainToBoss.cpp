#include "stdafx.h"
#include "Collider_MainToBoss.h"

// Client
#include "Player.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CCollider_MainToBoss::CCollider_MainToBoss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CCollider_MainToBoss::CCollider_MainToBoss(const CCollider_MainToBoss& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CCollider_MainToBoss::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCollider_MainToBoss::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector vecPos = { -15.f, 9.5f, 71.2f, 1.0f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

    return S_OK;
}

void CCollider_MainToBoss::Priority_Tick(_float _fTimeDelta)
{
}

void CCollider_MainToBoss::Tick(_float _fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CCollider_MainToBoss::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CCollider_MainToBoss::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

void CCollider_MainToBoss::Collision_ToPlayer()
{
    if (Collision_ToPlayerBody())
    {
        // Move To Boss Stage 
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
        if (nullptr == pPlayer)
            return;

        pPlayer->Set_IsBossStage(true);

        m_pGameInstance->StopSound(SOUNDTYPE::BGM);
        m_pGameInstance->PlayBGMLoop("1-09 Boss Battle (Eagle's Tower Version).mp3", 0.5f);
        _vector vPos = { 2992.5f, 40.f, -210.f, 1.0f };
        dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, vPos);
    }
}

_bool CCollider_MainToBoss::Collision_ToPlayerBody()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return false;

    return m_pColliderCom->Intersect(pPlayerCollider);
}

HRESULT CCollider_MainToBoss::Add_Component()
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

CCollider_MainToBoss* CCollider_MainToBoss::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCollider_MainToBoss* pInstance = new CCollider_MainToBoss(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCollider_MainToBoss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCollider_MainToBoss::Clone(void* _pArg, _uint _iLevel)
{
    CCollider_MainToBoss* pInstance = new CCollider_MainToBoss(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider_MainToBoss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_MainToBoss::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
