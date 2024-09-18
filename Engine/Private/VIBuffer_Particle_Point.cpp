#include "VIBuffer_Particle_Point.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer_Instancing(_pDevice, _pContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& _rhs)
	: CVIBuffer_Instancing(_rhs)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(_uint _iNumInstance)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    //////////////////////////////////
    /* Set Up Vertex Member Various */
    //////////////////////////////////
    {
        m_iNumVertexBuffers = 2;
        m_iVertexStride = sizeof(VTXPOINT);
        m_iNumVertices = 1;
    }

    /////////////////////////////////
    /* Set Up Index Member Various */
    /////////////////////////////////
    {
        m_iNumInstance = _iNumInstance;
        m_iIndexCountPerInstance = 1;
        m_iIndexStride = 2;
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
    }

    //////////////////////////
    /* Create Vertex Buffer */
    //////////////////////////
    VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);
    {
        pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);
        pVertices[0].vPSize = _float2(1.f, 1.f);

        /* D3D11_BUFFER_DESC m_BufferDesc */
        ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
        {
            m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
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
         /* D3D11_BUFFER_DESC m_BufferDesc */
        ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
        {
            m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
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

HRESULT CVIBuffer_Particle_Point::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //////////////////////////////////
    /* Set Up Instance Member Various */
    //////////////////////////////////
    {
        m_iInstanceStride = sizeof(VTXINSTANCE);
    }

    //////////////////////////
    /* Create Vertex Buffer */
    //////////////////////////
    VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];
    ZeroMemory(pVertices, sizeof(VTXINSTANCE) * m_iNumInstance);
    {
        uniform_real_distribution<float>	WidthRange(m_tInstanceData.vRange.x * -0.5f, m_tInstanceData.vRange.x * 0.5f);
        uniform_real_distribution<float>	HeightRange(m_tInstanceData.vRange.y * -0.5f, m_tInstanceData.vRange.y * 0.5f);
        uniform_real_distribution<float>	DepthRange(m_tInstanceData.vRange.z * -0.5f, m_tInstanceData.vRange.z * 0.5f);
        uniform_real_distribution<float>	SizeRange(m_tInstanceData.vSize.x, m_tInstanceData.vSize.y);

        for (_uint i = 0; i < m_iNumInstance/2; ++i)
        {
            _float		fSize = SizeRange(m_RandomNumber);

            pVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
            pVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
            pVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
            pVertices[i].vTranslation = _float4(
                m_tInstanceData.vCenter.x + WidthRange(m_RandomNumber),
                m_tInstanceData.vCenter.y + HeightRange(m_RandomNumber),
                m_tInstanceData.vCenter.z + DepthRange(m_RandomNumber),
                1.f);

            pVertices[i].vColor = m_tInstanceData.vColor;
        }

        /* D3D11_BUFFER_DESC m_BufferDesc */
        ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
        {
            m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
            m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_BufferDesc.MiscFlags = 0;
            m_BufferDesc.StructureByteStride = m_iVertexStride;
        }

        /* D3D11_SUBRESOURCE_DATA m_InitialData */
        ZeroMemory(&m_InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
        {
            m_InitialData.pSysMem = pVertices;
        }

        if (FAILED(__super::Create_Buffer(&m_pVBInstance)))
            return E_FAIL;

        Safe_Delete_Array(pVertices);
    }

    return S_OK;
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumInstance)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent* CVIBuffer_Particle_Point::Clone(void* _pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
	__super::Free();
}
