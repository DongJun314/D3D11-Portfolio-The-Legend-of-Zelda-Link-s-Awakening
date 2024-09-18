#include "stdafx.h"
#include "StalfosOrange.h"

CStalfosOrange::CStalfosOrange(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CStalfosOrange::CStalfosOrange(const CStalfosOrange& _rhs)
    : CMonster(_rhs)
{
}

HRESULT CStalfosOrange::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStalfosOrange::Initialize(void* _pArg)
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

    _matrix WorldMatrix = XMMatrixSet
    (0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        -45.04f, 14.f, 68.6f, 1.0f);

    m_pTransformCom->Set_WorldMatrix(WorldMatrix);

    m_pModelCom->Set_Init_Animation(3);

    return S_OK;
}

void CStalfosOrange::Priority_Tick(_float _fTimeDelta)
{
    Invalidate_ColliderTransformationMatrix();
}

void CStalfosOrange::Tick(_float _fTimeDelta)
{
    m_pModelCom->Set_Animation(3);
    m_pModelCom->Play_Animation(_fTimeDelta, true);
}

void CStalfosOrange::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();

    SetUp_OnTerrain(m_pTransformCom, m_pNavigationCom);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CStalfosOrange::Render()
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

//#ifdef _DEBUG
//    for (size_t i = 0; i < TYPE_END; i++)
//    {
//        m_pColliderCom[i]->Render();
//    }
//#endif

    return S_OK;
}

void CStalfosOrange::Collision_ToPlayer()
{
}

_bool CStalfosOrange::Collision_ToPlayerBody(MCOLLIDER _eType)
{
    return _bool();
}

_bool CStalfosOrange::Collision_ToPlayerSword(MCOLLIDER _eType)
{
    return _bool();
}

_bool CStalfosOrange::Collision_ToPlayerShield(MCOLLIDER _eType)
{
    return _bool();
}

void CStalfosOrange::Invalidate_ColliderTransformationMatrix()
{
}

HRESULT CStalfosOrange::Add_Component()
{	
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_StalfosOrange"),
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

    return S_OK;
}

HRESULT CStalfosOrange::Add_PartObjects()
{
    return S_OK;
}

HRESULT CStalfosOrange::Bind_ShaderResources()
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

void CStalfosOrange::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}


CStalfosOrange* CStalfosOrange::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CStalfosOrange* pInstance = new CStalfosOrange(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CStalfosOrange");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStalfosOrange::Clone(void* _pArg, _uint _iLevel)
{
    CStalfosOrange* pInstance = new CStalfosOrange(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CStalfosOrange");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStalfosOrange::Free()
{
    __super::Free();
}
