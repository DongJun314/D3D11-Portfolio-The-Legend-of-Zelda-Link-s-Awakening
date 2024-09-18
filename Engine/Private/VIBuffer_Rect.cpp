#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& _rhs)
    : CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{   
    //////////////////////////////////
    /* Set Up Vertex Member Various */
    //////////////////////////////////
    {
        m_iNumVertexBuffers = 1;
        m_iNumVertices = 4;
        m_iVertexStride = sizeof(VTXPOSTEX);
    }

    /////////////////////////////////
    /* Set Up Index Member Various */
    /////////////////////////////////
    {
        m_iNumIndices = 6;
        m_iIndexStride = 2; //6만 5천개를 넘어가지 않으면 2바이트면 충분하다
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    //////////////////////////
    /* Create Vertex Buffer */
    //////////////////////////
    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    {
        /* VTXPOSTEX* pVertices */
        ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
        {
            pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
            pVertices[0].vTexcoord = _float2(0.f, 0.f);

            pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
            pVertices[1].vTexcoord = _float2(1.f, 0.f);

            pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
            pVertices[2].vTexcoord = _float2(1.f, 1.f);

            pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
            pVertices[3].vTexcoord = _float2(0.f, 1.f);
        }

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
   {
       /* _ushort* pIndices */
       ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
       {
           pIndices[0] = 0;
           pIndices[1] = 1;
           pIndices[2] = 2;

           pIndices[3] = 0;
           pIndices[4] = 2;
           pIndices[5] = 3;
       }

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

HRESULT CVIBuffer_Rect::Initialize(void* _pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* _pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Rect::Free()
{
    __super::Free();
}
