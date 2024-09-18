#include "stdafx.h"
#include "Piccolo.h"

#include "Player.h"
#include "FireBall.h"
#include "Map_GameA.h"

CPiccolo::CPiccolo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CPiccolo::CPiccolo(const CPiccolo& _rhs)
    : CMonster(_rhs)
{
}

HRESULT CPiccolo::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPiccolo::Initialize(void* _pArg)
{
    LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)_pArg;
    {
        pGameObjectDesc->fSpeedPerSec = 1.f;
        pGameObjectDesc->fRotationPerSec = XMConvertToRadians(90.f);
    }

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_SpeedPerSec(1.f + rand() % 2);
    m_pTransformCom->LookAt(XMVectorSet(1.f, 0.f, 0.f, 1.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-35.f + rand() % 10, 30.f + rand() % 25, 2003.f + rand() % 25, 1.0f));

    m_pModelCom->Set_Init_Animation(0);

    return S_OK;
}

void CPiccolo::Priority_Tick(_float _fTimeDelta)
{
    Invalidate_ColliderTransformationMatrix();
}

void CPiccolo::Tick(_float _fTimeDelta)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return;

    if (pPlayer->Get_MiniGameA())
    {
        if (m_bHit)
        {
            m_pTransformCom->Go_Down(_fTimeDelta * 8.f);
            m_pModelCom->Set_Animation(1);
            m_pModelCom->Play_Animation(_fTimeDelta, true);
            return;
        }
        else
        {
            m_pTransformCom->Go_Straight(_fTimeDelta);
            m_pModelCom->Set_Animation(0);
            m_pModelCom->Play_Animation(_fTimeDelta, true);
            return;
        }
    }
}

void CPiccolo::Late_Tick(_float _fTimeDelta)
{
    Collision_ToFireBall();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CPiccolo::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

#ifdef _DEBUG
m_pColliderCom->Render();
#endif

    return S_OK;
}

void CPiccolo::Collision_ToFireBall()
{
    CMap_GameA* pGameA = dynamic_cast<CMap_GameA*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), 0));
    if (pGameA == nullptr)
        return;

    if (Collision_ToFireBallCollider())
    {
        m_bHit = true;

        if (!m_bFindBefore)
        {
            m_pGameInstance->StopSound(SOUNDTYPE::HIT);
            m_pGameInstance->PlaySoundEx("3_Monster_Hit.wav", SOUNDTYPE::HIT, 0.3f);
            pGameA->Add_HitCount(CMap_GameA::STAGE_4);
            m_bFindBefore = true;
        }
    }
}

_bool CPiccolo::Collision_ToFireBallCollider()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    _int iFireBallCloneCount = pPlayer->Get_FireBallCloneCount();
    if (iFireBallCloneCount == -1)
        return false;

    CFireBall* pFireBall = dynamic_cast<CFireBall*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_FireBall"), iFireBallCloneCount));
    if (nullptr == pFireBall)
        return false;

    CCollider* pFireBallCollider = dynamic_cast<CCollider*>(pFireBall->Get_Component(TEXT("Com_Collider")));
    if (nullptr == pFireBallCollider)
        return false;

    return m_pColliderCom->Intersect(pFireBallCollider);
}

void CPiccolo::Invalidate_ColliderTransformationMatrix()
{ 
    // 내 자신의 월드행렬에 부모의 월드행렬을 곱함으로써, 최종 월드 행렬을 가진다
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
    m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

HRESULT CPiccolo::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Piccolo"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Navigation */
    CNavigation::NAVI_DESC NaviDesc{};
    {
        NaviDesc.iStartCellIndex = 0;
    }
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Body Com_Collider */
    {
        CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
        {
            BodyBoundingDesc.fRadius = 0.35f;
            BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius - 0.05f, 0.f);
        }

        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPiccolo::Bind_ShaderResources()
{
    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    _float4 CamPos = m_pGameInstance->Get_CamPosition();

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
        return E_FAIL;

    const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

void CPiccolo::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}


CPiccolo* CPiccolo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPiccolo* pInstance = new CPiccolo(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPiccolo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPiccolo::Clone(void* _pArg, _uint _iLevel)
{
    CPiccolo* pInstance = new CPiccolo(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPiccolo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPiccolo::Free()
{
    __super::Free();
}
