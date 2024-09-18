#include "stdafx.h"
#include "FireBall.h"

#include "Player.h"

CFireBall::CFireBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CMonster(_pDevice, _pContext)
{
}

CFireBall::CFireBall(const CFireBall& _rhs)
    : CMonster(_rhs)
{
}

HRESULT CFireBall::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFireBall::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

    _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    m_vPlayerDir = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

    return S_OK;
}

void CFireBall::Priority_Tick(_float _fTimeDelta)
{
    
}

void CFireBall::Tick(_float _fTimeDelta)
{ 
    _vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fSpeed = 0.3f;
    vMonsterPos += m_vPlayerDir * fSpeed;
   
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonsterPos);
    Invalidate_ColliderTransformationMatrix();
}

void CFireBall::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CFireBall::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
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

void CFireBall::Collision_ToPlayer()
{
    /*if (Collision_ToPlayerBody(MCOLLIDER _eType))
    {
    }*/
}

_bool CFireBall::Collision_ToPlayerBody(MCOLLIDER _eType)
{
    /*  CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
      if (nullptr == pPlayer)
          return false;

      CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
      if (nullptr == pPlayerCollider)
          return false;

      return m_pColliderCom[TYPE_BODY]->Intersect(pPlayerCollider);*/
    return _bool();
}

_bool CFireBall::Collision_ToPlayerSword(MCOLLIDER _eType)
{
    return _bool();
}

_bool CFireBall::Collision_ToPlayerShield(MCOLLIDER _eType)
{
    return _bool();
}

void CFireBall::Invalidate_ColliderTransformationMatrix()
{
    // 내 자신의 월드행렬에 부모의 월드행렬을 곱함으로써, 최종 월드 행렬을 가진다
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
    m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

HRESULT CFireBall::Add_Component()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Body Com_Collider */
    {
        CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
        {
            BodyBoundingDesc.fRadius = 0.6f;
            BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius - 0.6f, 0.f);
        }

        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFireBall::Add_PartObjects()
{
    return S_OK;
}

HRESULT CFireBall::Bind_ShaderResources()
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

void CFireBall::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}


CFireBall* CFireBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFireBall* pInstance = new CFireBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CFireBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFireBall::Clone(void* _pArg, _uint _iLevel)
{
    CFireBall* pInstance = new CFireBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CFireBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFireBall::Free()
{
    __super::Free();
}