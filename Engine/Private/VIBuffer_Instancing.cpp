#include "VIBuffer_Instancing.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& _rhs)
    : CVIBuffer(_rhs)
    , m_pVBInstance(_rhs.m_pVBInstance)
    , m_iInstanceStride(_rhs.m_iInstanceStride)
    , m_iNumInstance(_rhs.m_iNumInstance)
    , m_iIndexCountPerInstance(_rhs.m_iIndexCountPerInstance)
    , m_RandomNumber(_rhs.m_RandomNumber)
    , m_pSpeed(_rhs.m_pSpeed)
{
    Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
    m_RandomNumber = mt19937_64(m_RandomDevice());

    return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* _pArg)
{
    m_tInstanceData = *(INSTANCE_DESC*)_pArg;

    m_pSpeed = new _float[m_iNumInstance];
    m_pLifeTime = new _float[m_iNumInstance];

    uniform_real_distribution<float> SpeedRange(m_tInstanceData.vSpeed.x, m_tInstanceData.vSpeed.y);
    uniform_real_distribution<float> TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);

    for (_uint i = 0; i < m_iNumInstance; i++)
    {
        m_pSpeed[i] = SpeedRange(m_RandomNumber);
        m_pLifeTime[i] = TimeRange(m_RandomNumber);
    }

    return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_Buffers()
{
    ID3D11Buffer* pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance
    };

    _uint iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceStride
    };

    _uint iOffsets[] = {
        0,
        0
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

void CVIBuffer_Instancing::Tick_Drop(_float _fTimeDelta)
{
    m_fTimeAcc += _fTimeDelta;

    if (m_tInstanceData.fDuration <= m_fTimeAcc)
        m_bIsFinished = true;

    D3D11_MAPPED_SUBRESOURCE			SubResource{};
    
    uniform_real_distribution<float>	TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);
    uniform_real_distribution<float>	WidthRange(m_tInstanceData.vRange.x * -0.5f, m_tInstanceData.vRange.x * 0.5f);
    uniform_real_distribution<float>	HeightRange(m_tInstanceData.vRange.y * -0.5f, m_tInstanceData.vRange.y * 0.5f);
    uniform_real_distribution<float>	DepthRange(m_tInstanceData.vRange.z * -0.5f, m_tInstanceData.vRange.z * 0.5f);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        ((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y -= m_pSpeed[i] * _fTimeDelta;

        m_pLifeTime[i] -= _fTimeDelta;
        if (0.0f >= m_pLifeTime[i])
        {
            if (true == m_tInstanceData.bIsLoop)
            {
                m_pLifeTime[i] = TimeRange(m_RandomNumber);
                ((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
                   m_tInstanceData.vCenter.x + WidthRange(m_RandomNumber),
                   m_tInstanceData.vCenter.y + HeightRange(m_RandomNumber),
                   m_tInstanceData.vCenter.z + DepthRange(m_RandomNumber),
                    1.f);
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.x = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.y = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.z = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 1.f;
            }
            else
            {           
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Spread(_float _fTimeDelta)
{
    m_fTimeAcc += _fTimeDelta;

    if (m_tInstanceData.fDuration <= m_fTimeAcc)
        m_bIsFinished = true;

    D3D11_MAPPED_SUBRESOURCE			SubResource{};

    uniform_real_distribution<float>	TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);
    uniform_real_distribution<float>	WidthRange(m_tInstanceData.vRange.x * -0.5f, m_tInstanceData.vRange.x * 0.5f);
    uniform_real_distribution<float>	HeightRange(m_tInstanceData.vRange.y * -0.5f, m_tInstanceData.vRange.y * 0.5f);
    uniform_real_distribution<float>	DepthRange(m_tInstanceData.vRange.z * -0.5f, m_tInstanceData.vRange.z * 0.5f);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

        _vector	vDir = XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_tInstanceData.vPivot);
        vDir = XMVectorSetW(vDir, 0.f);

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * _fTimeDelta);

        m_pLifeTime[i] -= _fTimeDelta;
        if (0.0f >= m_pLifeTime[i])
        {
            if (true == m_tInstanceData.bIsLoop)
            {
                m_pLifeTime[i] = TimeRange(m_RandomNumber);
                ((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
                    m_tInstanceData.vCenter.x + WidthRange(m_RandomNumber),
                    m_tInstanceData.vCenter.y + HeightRange(m_RandomNumber),
                    m_tInstanceData.vCenter.z + DepthRange(m_RandomNumber),
                    1.f);
            }
            else
            {
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Charge(_float _fTimeDelta)
{
    m_fTimeAcc += _fTimeDelta;

    if (m_tInstanceData.fDuration <= m_fTimeAcc)
        m_bIsFinished = true;

    D3D11_MAPPED_SUBRESOURCE			SubResource{};

    uniform_real_distribution<float>	TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);
    uniform_real_distribution<float>	WidthRange(m_tInstanceData.vRange.x * -0.5f, m_tInstanceData.vRange.x * 0.5f);
    uniform_real_distribution<float>	HeightRange(m_tInstanceData.vRange.y * -0.5f, m_tInstanceData.vRange.y * 0.5f);
    uniform_real_distribution<float>	DepthRange(m_tInstanceData.vRange.z * -0.5f, m_tInstanceData.vRange.z * 0.5f);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

        _vector	vDir = XMLoadFloat3(&m_tInstanceData.vPivot) - XMLoadFloat4(&pVertices[i].vTranslation);

        vDir = XMVectorSetW(vDir, 0.f);

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + XMVector3Normalize(vDir) * m_pSpeed[i] * _fTimeDelta);

        m_pLifeTime[i] -= _fTimeDelta;
        if (0.0f >= m_pLifeTime[i])
        {
            if (true == m_tInstanceData.bIsLoop)
            {
                m_pLifeTime[i] = TimeRange(m_RandomNumber);
                ((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
                    m_tInstanceData.vCenter.x + WidthRange(m_RandomNumber),
                    m_tInstanceData.vCenter.y + HeightRange(m_RandomNumber),
                    m_tInstanceData.vCenter.z + DepthRange(m_RandomNumber),
                    1.f);
            }
            else
            {
                ((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Tick_Reset(_float _fTimeDelta)
{
    /*
    * // 추후 혹시 Reset에 문제가 생기면 아마 초기화를 안해줘서 그럴듯하다. 현재는 초기화를 안해도 잘된다
     uniform_real_distribution<float> TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);

     for (_uint i = 0; i < m_iNumInstance; i++)
     {
         m_pLifeTime[i] = TimeRange(m_RandomNumber);
     }
    */

    m_fTimeAcc += _fTimeDelta;

    if (m_tInstanceData.fDuration <= m_fTimeAcc)
        m_bIsFinished = true;

    D3D11_MAPPED_SUBRESOURCE			SubResource{};

    uniform_real_distribution<float>	TimeRange(m_tInstanceData.vLifeTime.x, m_tInstanceData.vLifeTime.y);
    uniform_real_distribution<float>	WidthRange(m_tInstanceData.vRange.x * -0.5f, m_tInstanceData.vRange.x * 0.5f);
    uniform_real_distribution<float>	HeightRange(m_tInstanceData.vRange.y * -0.5f, m_tInstanceData.vRange.y * 0.5f);
    uniform_real_distribution<float>	DepthRange(m_tInstanceData.vRange.z * -0.5f, m_tInstanceData.vRange.z * 0.5f);

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    for (_uint i = 0; i < m_iNumInstance; ++i)
    {
        VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

        m_pLifeTime[i] -= _fTimeDelta;
        if (0.0f >= m_pLifeTime[i])
        {
            m_pLifeTime[i] = TimeRange(m_RandomNumber);
            ((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(
                m_tInstanceData.vCenter.x + WidthRange(m_RandomNumber),
                m_tInstanceData.vCenter.y + HeightRange(m_RandomNumber),
                m_tInstanceData.vCenter.z + DepthRange(m_RandomNumber),
                1.f);
            ((VTXINSTANCE*)SubResource.pData)[i].vColor.w = 1.f;
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Instancing::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pLifeTime);
    Safe_Delete_Array(m_pSpeed);

    Safe_Release(m_pVBInstance);
}
