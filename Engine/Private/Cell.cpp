#include "Cell.h"

// Engine
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* _pPoints, _uint _iCellIndex)
{
    for (size_t i = 0; i < POINT_END; i++)
        m_vPoints[i] = _pPoints[i];

    m_iIndex = _iCellIndex;
    

    XMStoreFloat3(&m_vNormals[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
    XMStoreFloat3(&m_vNormals[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
    XMStoreFloat3(&m_vNormals[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));

    _float3 vNormalAB = _float3(m_vNormals[LINE_AB].z * -1.f, 0.f, m_vNormals[LINE_AB].x);
    _float3 vNormalBC = _float3(m_vNormals[LINE_BC].z * -1.f, 0.f, m_vNormals[LINE_BC].x);
    _float3 vNormalCA = _float3(m_vNormals[LINE_CA].z * -1.f, 0.f, m_vNormals[LINE_CA].x);
    
    XMStoreFloat3(&m_vNormals[LINE_AB], XMVector3Normalize(XMLoadFloat3(&vNormalAB)));
    XMStoreFloat3(&m_vNormals[LINE_BC], XMVector3Normalize(XMLoadFloat3(&vNormalBC)));
    XMStoreFloat3(&m_vNormals[LINE_CA], XMVector3Normalize(XMLoadFloat3(&vNormalCA)));


#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool CCell::Compare_Points(const _float3& _vSourPoint, const _float3& _vDestPoint)
{
    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&_vSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&_vDestPoint)))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&_vDestPoint)))
            return true;
    }
    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&_vSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&_vDestPoint)))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&_vDestPoint)))
            return true;
    }
    if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&_vSourPoint)))
    {
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&_vDestPoint)))
            return true;
        if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&_vDestPoint)))
            return true;
    }

    return false;
}

_bool CCell::Is_In(_fvector _vLocalPos, _int* _pNeighborIndex)
{
    for (size_t i = 0; i < LINE_END; i++)
    {
        _vector vDir = XMVector3Normalize(_vLocalPos - XMLoadFloat3(&m_vPoints[i]));

        // 현재 우리 코드상으로는 내적 값이 0 보다 클 경우, 셀 안에 있는것이며, 0 보다 작을 경우 셀 안에 있는 것이다
        if (0 < XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_vNormals[i]), vDir)))
        {
            *_pNeighborIndex = m_iNeighborIndices[i];
            return false;
        }
    }

    return true;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_Buffers();
    m_pVIBuffer->Render();

    return S_OK;
}
#endif 

CCell* CCell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, _uint _iCellIndex)
{
    CCell* pInstance = new CCell(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pPoints, _iCellIndex)))
    {
        MSG_BOX("Failed to Created : CCell");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif 
}
