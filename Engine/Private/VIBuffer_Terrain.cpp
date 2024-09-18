#include "VIBuffer_Terrain.h"

// Engine
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer(_pDevice, _pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& _rhs)
    : CVIBuffer(_rhs)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& _strHeightMapFilePath)
{
    _ulong		dwByte = { 0 };
    HANDLE		hFile = CreateFile(_strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    // 파일에 대한 정보를 가지는거, 파일의 크기, 생성된 년도
    BITMAPFILEHEADER			fh;
    ::ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);

    // 이미지에 대한 정보, 이미지 하나는 몇 픽셀, 이미지는 가로 몇 픽셀..
    BITMAPINFOHEADER			ih;
    ::ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

    // 실제 픽셀의 정보들
    _uint* pPixels = new _uint[ih.biWidth * ih.biHeight];
    ::ReadFile(hFile, pPixels, sizeof(_uint) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

    CloseHandle(hFile);

    //////////////////////////////////
    /* Set Up Vertex Member Various */
    //////////////////////////////////
    {
        m_iNumVerticesX = ih.biWidth;
        m_iNumVerticesZ = ih.biHeight;

        m_iNumVertexBuffers = 1;
        m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
        m_iVertexStride = sizeof(VTXNORTEX);

        // 정점들의 위치를 저장하기 위한 동적배열, 높이맵을 구하기 위해서 추가해둠
        m_pVerticesPos = new _float3[m_iNumIndices];
    }

    /////////////////////////////////
    /* Set Up Index Member Various */
    /////////////////////////////////
    {
        m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
        m_iIndexStride = 4; //6만 5천개를 넘어가지 않으면 2바이트면 충분하다
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }


    //////////////////////////
    /* Create Vertex Buffer */
    //////////////////////////

    m_pVertices = new VTXNORTEX[m_iNumVertices];
    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    {
        /* VTXNORTEX* pVertices */
        ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);
        {
            for (_uint i = 0; i < m_iNumVerticesZ; i++)
            {
                for (_uint j = 0; j < m_iNumVerticesX; j++)
                {
                    _uint iIndex = i * m_iNumVerticesX + j;

                    pVertices[iIndex].vPosition = _float3((float)j, (pPixels[iIndex] & 0x000000ff) / 10.0f, (float)i);
                    pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
                    pVertices[iIndex].vTexcoord = _float2((float)j / (m_iNumVerticesX - 1.f), (float)i / (m_iNumVerticesZ - 1.f));

                    m_pVertices[iIndex].vPosition = _float3((float)j, (pPixels[iIndex] & 0x000000ff) / 10.0f, (float)i);
                    m_pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
                    m_pVertices[iIndex].vTexcoord = _float2((float)j / (m_iNumVerticesX - 1.f), (float)i / (m_iNumVerticesZ - 1.f));
                }
            }
        }

        
        
    }

    /////////////////////////
    /* Create Index Buffer */
    /////////////////////////
    m_pIndices = new _uint[m_iNumIndices];
    _uint* pIndices = new _uint[m_iNumIndices];
    {
        /* _uint* pIndices */
        ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);
        {
            _uint		iNumIndices = { 0 };
            _uint       iNumIndices1 = { 0 };

            for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
            {
                for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
                {
                    _uint iIndex = j * m_iNumVerticesX + i;

                    _uint	iIndices[4] = {
                        iIndex + m_iNumVerticesX,
                        iIndex + m_iNumVerticesX + 1,
                        iIndex + 1,
                        iIndex
                    };

                    _vector vSourDir, vDestDir, vNormal;

                    pIndices[iNumIndices++] = iIndices[0];
                    pIndices[iNumIndices++] = iIndices[1];
                    pIndices[iNumIndices++] = iIndices[2];

                    m_pIndices[iNumIndices1++] = iIndices[0];
                    m_pIndices[iNumIndices1++] = iIndices[1];
                    m_pIndices[iNumIndices1++] = iIndices[2];

                    vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
                    vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
                    vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

                    XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
                    XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
                    XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));


                    XMStoreFloat3(&m_pVertices[iIndices[0]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[0]].vNormal) + vNormal));
                    XMStoreFloat3(&m_pVertices[iIndices[1]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[1]].vNormal) + vNormal));
                    XMStoreFloat3(&m_pVertices[iIndices[2]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[2]].vNormal) + vNormal));


                    pIndices[iNumIndices++] = iIndices[0];
                    pIndices[iNumIndices++] = iIndices[2];
                    pIndices[iNumIndices++] = iIndices[3];

                    m_pIndices[iNumIndices1++] = iIndices[0];
                    m_pIndices[iNumIndices1++] = iIndices[2];
                    m_pIndices[iNumIndices1++] = iIndices[3];

                    vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
                    vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
                    vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));               


                    XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
                    XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
                    XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));

                    XMStoreFloat3(&m_pVertices[iIndices[0]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[0]].vNormal) + vNormal));
                    XMStoreFloat3(&m_pVertices[iIndices[2]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[2]].vNormal) + vNormal));
                    XMStoreFloat3(&m_pVertices[iIndices[3]].vNormal,
                        XMVector3Normalize(XMLoadFloat3(&m_pVertices[iIndices[3]].vNormal) + vNormal));
                }
            }
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

    // Safe_AddRef
    {
        //memcpy(m_pVertices, pVertices, sizeof(VTXNORTEX));
       // memcpy(m_pIndices, pIndices, sizeof(_uint));
    
       //m_pVertices = new VTXNORTEX[m_iNumVertices];
     
      /* for (size_t i = 0; i < m_iNumVertices; i++)
       {
           m_pVertices[i] = pVertices[i];
       }*/

         

       /*m_pIndices = new _uint[m_iNumIndices];

       for (size_t i = 0; i < m_iNumIndices; i++)
       {
           m_pIndices[i] = pIndices[i];
       }*/
    }
     
    // Safe Release
    {
        Safe_Delete_Array(pPixels);
        Safe_Delete_Array(pVertices);
        Safe_Delete_Array(pIndices);
    }

   
    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* _pArg)
{
    return S_OK;
}

#include "iomanip"

_float CVIBuffer_Terrain::Compute_Height(_fvector _vLocalPos)
{
    _float3 vTargetPos;
    XMStoreFloat3(&vTargetPos, _vLocalPos);

    _uint iIndex = _uint(vTargetPos.z) * m_iNumVerticesX + _uint(vTargetPos.x);

    _uint iIndices[4] = {
        iIndex + m_iNumVerticesX,
        iIndex + m_iNumVerticesX + 1,
        iIndex + 1,
        iIndex
    };

    _float fWidth = vTargetPos.x - m_pVerticesPos[iIndices[0]].x;
    _float fDepth = m_pVerticesPos[iIndices[0]].z - vTargetPos.z;

    _vector vPlane;

    _float x0 = (_float)(round(m_pVerticesPos[iIndices[0]].x * 1e2) / 1e2);
    _float y0 = (_float)(round(m_pVerticesPos[iIndices[0]].x * 1e2) / 1e2);
    _float z0 = (_float)(round(m_pVerticesPos[iIndices[0]].x * 1e2) / 1e2);

    _float x1 = (_float)(round(m_pVerticesPos[iIndices[1]].x * 1e2) / 1e2);
    _float y1 = (_float)(round(m_pVerticesPos[iIndices[1]].x * 1e2) / 1e2);
    _float z1 = (_float)(round(m_pVerticesPos[iIndices[1]].x * 1e2) / 1e2);

    _float x2 = (_float)(round(m_pVerticesPos[iIndices[2]].x * 1e2) / 1e2);
    _float y2 = (_float)(round(m_pVerticesPos[iIndices[2]].x * 1e2) / 1e2);
    _float z2 = (_float)(round(m_pVerticesPos[iIndices[2]].x * 1e2) / 1e2);

    _float x3 = (_float)(round(m_pVerticesPos[iIndices[3]].x * 1e2) / 1e2);
    _float y3 = (_float)(round(m_pVerticesPos[iIndices[3]].x * 1e2) / 1e2);
    _float z3 = (_float)(round(m_pVerticesPos[iIndices[3]].x * 1e2) / 1e2);


    if (fWidth > fDepth)
    {

        vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]));
    }
    else
    {
        vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), XMLoadFloat3(&m_pVerticesPos[iIndices[3]]));
    }

    _float x = vPlane.m128_f32[0];
    _float y = vPlane.m128_f32[1];
    _float z = vPlane.m128_f32[2];
    _float w = vPlane.m128_f32[3];

    _float a = (-x * vTargetPos.x - z * vTargetPos.z - w) / y;

    return (-x * vTargetPos.x - z * vTargetPos.z - w) / y;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_strHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* _pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();
    
    Safe_Delete_Array(m_pVertices);
    Safe_Delete_Array(m_pIndices);
}
