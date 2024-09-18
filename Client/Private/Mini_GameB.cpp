#include "stdafx.h"
#include "Mini_GameB.h"

// Client
#include "Player.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CMini_GameB::CMini_GameB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMini_GameB::CMini_GameB(const CMini_GameB& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CMini_GameB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMini_GameB::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_vector vecPos = { -32.2f, 10.5f, 59.7f, 1.0f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);
	
	return S_OK;
}

void CMini_GameB::Priority_Tick(_float _fTimeDelta)
{
}

void CMini_GameB::Tick(_float _fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CMini_GameB::Late_Tick(_float _fTimeDelta)
{
	Collision_ToPlayer();
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMini_GameB::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CMini_GameB::Collision_ToPlayer()
{
	if (Collision_ToPlayerBody())
	{
		// Set Next Scene
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
		if (nullptr == pPlayer)
			return;

		_vector Pos = { -178.f, 33.7f, -1973.f, 1.f };

		dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, Pos);
	}
}

_bool CMini_GameB::Collision_ToPlayerBody()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return false;

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
	if (nullptr == pPlayerCollider)
		return false;

	return m_pColliderCom->Intersect(pPlayerCollider);
}


HRESULT CMini_GameB::Add_Component()
{
	/* Scene Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
		{
			BodyBoundingDesc.fRadius = 0.8f;
			BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
			return E_FAIL;
	}

	return S_OK;
}

CMini_GameB* CMini_GameB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMini_GameB* pInstance = new CMini_GameB(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMini_GameB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMini_GameB::Clone(void* _pArg, _uint _iLevel)
{
	CMini_GameB* pInstance = new CMini_GameB(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMini_GameB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMini_GameB::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
