#include "stdafx.h"
#include "Particle_Rect.h"

#include "Player.h"

CParticle_Rect::CParticle_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CParticle_Rect::CParticle_Rect(const CParticle_Rect& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CParticle_Rect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Rect::Initialize(void* _pArg)
{
    CGameObject::GAMEOBJECT_DESC			GameObjectDesc = {};
    {
        GameObjectDesc.iGameObjectData = 10;
        GameObjectDesc.fSpeedPerSec = 10.0f;
        GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);
    }

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.58f, 12.5f, 59.7f, 1.0f));

    return S_OK;
}

void CParticle_Rect::Priority_Tick(_float _fTimeDelta)
{
   /* CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform")));
    if (nullptr == pPlayerTransform)
        return;

    _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

    vPlayerPos.m128_f32[1] += 1.f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);*/
}

void CParticle_Rect::Tick(_float _fTimeDelta)
{
    //m_pVIBufferCom->Tick_Drop(_fTimeDelta);
    /*if (FAILED(Set_BillBoard()))
        return;*/

    //m_pVIBufferCom->Tick_Spread(_fTimeDelta);
   // m_pVIBufferCom->Tick_Charge(_fTimeDelta);
}

void CParticle_Rect::Late_Tick(_float _fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CParticle_Rect::Render()
{
    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Rect::Set_BillBoard()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (pPlayer == nullptr)
        return E_FAIL;

    _bool bIsFirstView = pPlayer->Get_FirstView();

    _matrix WorldMatrix, WorldMatrixX, WorldMatrixY;

    _double dAngleX, dAngleY;
    _float fRotationX, fRotationY;
    _float4 vCamPos = m_pGameInstance->Get_CamPosition();

    _float4 vPos = { 0.f ,0.f, 0.f, 0.f };
    XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

    if (bIsFirstView)
    {
        dAngleY = atan2(vPos.x - vCamPos.x, vPos.z - vCamPos.z) * (180.0 / ((FLOAT)3.141592654f));
        // 라디안 값으로 취환합니다
        fRotationY = (_float)dAngleY * 0.0174532925f;

        WorldMatrixY = XMMatrixRotationY(fRotationY);
        WorldMatrix = WorldMatrixY;
    }
    else
    {
        dAngleX = atan2(vPos.y - vCamPos.y, vPos.z - vCamPos.z) * (180.0 / ((FLOAT)3.141592654f));
        // 라디안 값으로 취환합니다
        fRotationX = (_float)-dAngleX * 0.0174532925f;

        WorldMatrixX = XMMatrixRotationX(fRotationX);
        WorldMatrix = WorldMatrixX;
    }

    _matrix TranslationMatrix = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
    WorldMatrix = XMMatrixMultiply(WorldMatrix, TranslationMatrix);

    m_pTransformCom->Set_WorldMatrix(WorldMatrix);

    return S_OK;
}

HRESULT CParticle_Rect::Add_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    // Tick_Drop용
    //CVIBuffer_Instancing::INSTANCE_DESC InstanceDesc{};
    //{
    //    InstanceDesc.vPivot = _float3(10.f, 19.f, 0.f);
    //    //InstanceDesc.vCenter = _float3(1.58f, 13.5f, 59.7f);
    //    InstanceDesc.vCenter = _float3(0.0f, 0.f, 0.f);
    //    InstanceDesc.vRange = _float3(10.0f, 2.f, 10.f);
    //    InstanceDesc.vSize = _float2(0.2f, 0.2f);
    //    InstanceDesc.vSpeed = _float2(2.f, 5.f);
    //    InstanceDesc.vLifeTime = _float2(3.f, 5.f);
    //    InstanceDesc.bIsLoop = true;
    //    InstanceDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);
    //}

    // Tick_Spread용
    //CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
    //{
    //    InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
    //    InstanceDesc.vCenter = _float3(0.0f, 0.f, 0.f);
    //    InstanceDesc.vRange = _float3(1.3f, 1.3f, 1.3f);
    //    InstanceDesc.vSize = _float2(0.1f, 0.1f);
    //    InstanceDesc.vSpeed = _float2(2.f, 5.f);
    //    InstanceDesc.vLifeTime = _float2(3.f, 5.f);
    //    InstanceDesc.bIsLoop = true;
    //    InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    //}

    CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
    {
        InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
        InstanceDesc.vCenter = _float3(0.0f, 0.f, 0.f);
        InstanceDesc.vRange = _float3(10.3f, 10.3f, 10.3f);
        InstanceDesc.vSize = _float2(0.1f, 0.1f);
        InstanceDesc.vSpeed = _float2(2.f, 5.f);
        InstanceDesc.vLifeTime = _float2(3.f, 5.f);
        InstanceDesc.bIsLoop = true;
        InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    }

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstanceDesc)))
        return E_FAIL;

    return S_OK;
}

CParticle_Rect* CParticle_Rect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CParticle_Rect* pInstance = new CParticle_Rect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CParticle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Rect::Clone(void* _pArg, _uint _iLevel)
{
    CParticle_Rect* pInstance = new CParticle_Rect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CParticle_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Rect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
