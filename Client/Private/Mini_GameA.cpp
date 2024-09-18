#include "stdafx.h"
#include "Mini_GameA.h"

// Client
#include "Player.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CMini_GameA::CMini_GameA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CMini_GameA::CMini_GameA(const CMini_GameA& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CMini_GameA::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMini_GameA::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    _vector vecPos = { -3.6f, 12.7f, 85.5f, 1.0f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

    return S_OK;
}

void CMini_GameA::Priority_Tick(_float _fTimeDelta)
{
}

void CMini_GameA::Tick(_float _fTimeDelta)
{
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMini_GameA::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CMini_GameA::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif
    return S_OK;
}

void CMini_GameA::Collision_ToPlayer()
{
	if (Collision_ToPlayerBody())
	{
        // Move To MiniGameA 
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
        if (nullptr == pPlayer)
            return;

        pPlayer->Set_MiniGameA(true);

        m_pGameInstance->StopSound(SOUNDTYPE::BGM);
        m_pGameInstance->PlayBGMLoop("1-22 Mini Boss Battle.mp3",0.1f);

        _vector Pos = { 2.2f, 26.9f, 1994.5f, 1.f };
        dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, Pos);    
	}
}

_bool CMini_GameA::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}

HRESULT CMini_GameA::Add_Component()
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

        CBounding_Sphere::SPHERE_DESC		BackBoundingDesc{};
        {
            BackBoundingDesc.fRadius = 0.8f;
            BackBoundingDesc.vCenter = _float3(0.f, BackBoundingDesc.fRadius, 0.f);
        }

        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
            TEXT("Com_Back_Collider"), reinterpret_cast<CComponent**>(&m_pBack_ColliderCom), &BackBoundingDesc)))
            return E_FAIL;
    }

    return S_OK;
}

CMini_GameA* CMini_GameA::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMini_GameA* pInstance = new CMini_GameA(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMini_GameA");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMini_GameA::Clone(void* _pArg, _uint _iLevel)
{
    CMini_GameA* pInstance = new CMini_GameA(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CMini_GameA");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMini_GameA::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
