#include "Collider.h"

// Engine
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
{
}

CCollider::CCollider(const CCollider& _rhs)
    : CComponent(_rhs)
    , m_eType(_rhs.m_eType)
    , m_pBounding(_rhs.m_pBounding)
#ifdef _DEBUG
    , m_pBatch(_rhs.m_pBatch)
    , m_pEffect(_rhs.m_pEffect)
    , m_pInputLayout(_rhs.m_pInputLayout)
#endif
{
    Safe_AddRef(m_pBounding);

#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif
}

HRESULT CCollider::Initialize_Prototype(TYPE _eType)
{
    m_eType = _eType;

#ifdef _DEBUG
    m_pEffect = new BasicEffect(m_pDevice);
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t		iByteCodeLength = { 0 };

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
        pShaderByteCode, iByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CCollider::Initialize(void* _pArg)
{
    switch (m_eType)
    {
    case TYPE_AABB: 
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, _pArg);
        break;
    case TYPE_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, _pArg);
        break;
    case TYPE_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, _pArg);
        break;
    }

    return S_OK;
}

void CCollider::Tick(_fmatrix _WorldMatrix)
{
    m_pBounding->Tick(_WorldMatrix);
}

HRESULT CCollider::Render()
{
    // 셰이더 파일들은 서로 공유하기 때문에 이렇게 명시적으로 Geometry Shader를 사용안한다고 설정해줘야한다 
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pEffect->Apply(m_pContext);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch);

    m_pBatch->End();

    return S_OK;
}

_bool CCollider::Intersect(CCollider* _pTargetCollier)
{
    return m_pBounding->Intersect(_pTargetCollier->m_eType, _pTargetCollier->m_pBounding);
}

CCollider* CCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eType)
{
    CCollider* pInstance = new CCollider(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_eType)))
    {
        MSG_BOX("Failed to Created : CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider::Clone(void* _pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    __super::Free();

#ifdef _DEBUG

    if (false == m_bIsCloned)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

    Safe_Release(m_pInputLayout);

#endif

    Safe_Release(m_pBounding);
}
