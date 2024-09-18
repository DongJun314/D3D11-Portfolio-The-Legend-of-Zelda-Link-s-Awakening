#include "VIBuffer_Cell.h"

CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& _rhs)
    : CVIBuffer(_rhs)   
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* _pPoints)
{
    //////////////////////////////////
    /* Set Up Vertex Member Various */
    //////////////////////////////////
    {
        m_iNumVertexBuffers = 1;
        m_iNumVertices = 3;
        m_iVertexStride = sizeof(VTXPOS);
        m_pVerticesPos = new _float3[m_iNumVertices];
    }

    /////////////////////////////////
    /* Set Up Index Member Various */
    /////////////////////////////////
    {
        m_iNumIndices = 4;
        m_iIndexStride = 2; //6만 5천개를 넘어가지 않으면 2바이트면 충분하다
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }

    //////////////////////////
    /* Create Vertex Buffer */
    //////////////////////////
    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);
    {
        pVertices[0].vPosition = m_pVerticesPos[0] = _pPoints[0];
        pVertices[1].vPosition = m_pVerticesPos[1] = _pPoints[1];
        pVertices[2].vPosition = m_pVerticesPos[2] = _pPoints[2];
     
        /* D3D11_BUFFER_DESC m_BufferDesc */
        ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
        {
            m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
            m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
            m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            m_BufferDesc.CPUAccessFlags = 0;
            m_BufferDesc.MiscFlags = 0;
            m_BufferDesc.StructureByteStride = m_iVertexStride;
        }

        /* D3D11_SUBRESOURCE_DATA m_InitialData */
        ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
        {
            m_InitialData.pSysMem = pVertices;
        }

        if (FAILED(__super::Create_Buffer(&m_pVB)))
            return E_FAIL;
    }

    /////////////////////////
    /* Create Index Buffer */
    /////////////////////////
    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
    {
        // Index가 4개인 이유 : VIBuffer_Cell이라는 클래스는 내부를 채울 Texcoord가 필요없기에
        // Line Strip으로 삼각형 그리면 인덱스 4개 ((0, 1), (2, 0)) -> 이전 인덱스와 이어서 그림
        pIndices[0] = 0;
        pIndices[1] = 1;
        pIndices[2] = 2;
        pIndices[3] = 0;

        /* D3D11_BUFFER_DESC m_BufferDesc */
        ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
        {
            m_BufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
            m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
            m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            m_BufferDesc.CPUAccessFlags = 0;
            m_BufferDesc.MiscFlags = 0;
            m_BufferDesc.StructureByteStride = 0;
        }

        /* D3D11_SUBRESOURCE_DATA m_InitialData */
        ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
        {
            m_InitialData.pSysMem = pIndices;
        }

        if (FAILED(__super::Create_Buffer(&m_pIB)))
            return E_FAIL;
    }

    // Safe Release
    {
        Safe_Delete_Array(pVertices);
        Safe_Delete_Array(pIndices);
    }

    return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* _pArg)
{
    return S_OK;
}

#include "iomanip"

_float CVIBuffer_Cell::Compute_Height(_fvector _vLocalPos)
{
    _float3 vTargetPos;
    XMStoreFloat3(&vTargetPos, _vLocalPos);

    _float fWidth = vTargetPos.x - m_pVerticesPos[0].x;
    _float fDepth = m_pVerticesPos[0].z - vTargetPos.z;

    _vector vPlane;
    {
        vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[0]), XMLoadFloat3(&m_pVerticesPos[1]), XMLoadFloat3(&m_pVerticesPos[2]));
    }

    return (-vPlane.m128_f32[0] * vTargetPos.x - vPlane.m128_f32[2] * vTargetPos.z - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pPoints)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* _pArg)
{
	CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cell::Free()
{
	__super::Free();
}
