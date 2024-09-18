#include "Mesh.h"

// Engine
#include "Bone.h"
#include "Picking.h"
#include "GameInstance.h"

#include <fstream>

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CVIBuffer(_pDevice, _pContext)
{
}

CMesh::CMesh(const CMesh& _rhs)
	: CVIBuffer(_rhs)
{
}

HRESULT CMesh::Initialize_Prototype(std::ifstream* _pReadBinary, CModel* _pModel, CModel::TYPE _eType, _fmatrix _PivotMatrix)
{      
    HRESULT hr = _eType == CModel::TYPE_NONANIM ? Ready_Vertex_Buffer_NonAnim(_pReadBinary, _PivotMatrix) : Ready_Vertex_Buffer_Anim(_pReadBinary, _pModel);
    if (FAILED(hr))
        return E_FAIL; 

	return S_OK;
}

HRESULT CMesh::Initialize(void* _pArg)
{
	return S_OK;
}

void CMesh::SetUp_BoneMatrices(_float4x4* _pBoneMatrices, vector<class CBone*>& _vecBones)
{
    for (size_t i = 0; i < m_iNumBones; i++)
    {
        _float4x4 ComTransMatrix = _vecBones[m_vecBoneIndices[i]]->Get_CombinedTransformationMatrix();
        XMStoreFloat4x4(&_pBoneMatrices[i], XMLoadFloat4x4(&m_vecOffsetMatrices[i]) * XMLoadFloat4x4(&ComTransMatrix));
    }
}

HRESULT CMesh::Ready_Vertex_Buffer_NonAnim(std::ifstream* _pReadBinary, _fmatrix _PivotMatrix)
{



#pragma region BINARY_DATA

    _uint   iMaterialIndex;
    _pReadBinary->read(reinterpret_cast<char*>(&iMaterialIndex), sizeof(_uint));

    _char    szName[MAX_PATH] = "";
    _pReadBinary->read(reinterpret_cast<char*>(szName), sizeof(szName));

    // 정점 개수·
    _uint iNumVertices;
    _pReadBinary->read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));

    // 인덱스 개수
    _uint iNumIndices;
    _pReadBinary->read(reinterpret_cast<char*>(&iNumIndices), sizeof(_uint));

    //// 3-5 정점 정보
    VTXMESH* pVertices = new VTXMESH[iNumVertices];
    _pReadBinary->read(reinterpret_cast<char*>(pVertices), sizeof(VTXMESH) * iNumVertices);

    //// 인덱스 정보
    _ulong* pIndices = new _ulong[iNumIndices];
    _pReadBinary->read(reinterpret_cast<char*>(pIndices), sizeof(_ulong) * iNumIndices);

#pragma endregion






#pragma region MEMBER_VARIABLE
    
    // Setting Member Variable
    {
        m_iMaterialIndex = iMaterialIndex;
        strcpy_s(m_szName, szName);

        // Vertex
        m_iNumVertexBuffers = 1;
        m_iVertexStride = sizeof(VTXMESH);
        m_iNumVertices = iNumVertices;

        // Index
        m_iIndexStride = 4;
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_iNumIndices = iNumIndices;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

#pragma endregion  






#pragma region VERTEX_BUFFER

    /* D3D11_BUFFER_DESC m_BufferDesc */
    ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
    {
        /* 정점버퍼를 만든다.*/
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

#pragma endregion






#pragma region INDEX_BUFFER

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

#pragma endregion 






#pragma region PICKING

    // For Picking
    {
        m_pVertices = new VTXMESH[m_iNumVertices];
        for (size_t i = 0; i < m_iNumVertices; i++)
        {
            m_pVertices[i] = pVertices[i];
        }

        m_pIndices = new _uint[m_iNumIndices];
        for (size_t i = 0; i < m_iNumIndices; i++)
        {
            m_pIndices[i] = pIndices[i];
        }
    }

    // Safe Release
    {
        Safe_Delete_Array(pVertices);
        Safe_Delete_Array(pIndices);
    }

#pragma endregion



    return S_OK;
}

HRESULT CMesh::Ready_Vertex_Buffer_Anim(std::ifstream* _pReadBinary, CModel* _pModel)
{




#pragma region BINARY_DATA

    _uint   iMaterialIndex;
    _pReadBinary->read(reinterpret_cast<char*>(&iMaterialIndex), sizeof(_uint));

    _char    szName[MAX_PATH] = "";
    _pReadBinary->read(reinterpret_cast<char*>(szName), sizeof(szName));

    // 정점 개수·
    _uint iNumVertices;
    _pReadBinary->read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));

    // 인덱스 개수
    _uint iNumIndices;
    _pReadBinary->read(reinterpret_cast<char*>(&iNumIndices), sizeof(_uint));

    //// 3-5 정점 정보
    VTXANIMMESH* pVertices = new VTXANIMMESH[iNumVertices];
    _pReadBinary->read(reinterpret_cast<char*>(pVertices), sizeof(VTXANIMMESH) * iNumVertices);

    _uint iNumBones;
    _pReadBinary->read(reinterpret_cast<char*>(&iNumBones), sizeof(_uint));

    size_t OffsetMatricesSize;
    _pReadBinary->read(reinterpret_cast<char*>(&OffsetMatricesSize), sizeof(size_t));

    for (size_t j = 0; j < OffsetMatricesSize; j++)
    {
        _float4x4 OffsetMatrix;
        _pReadBinary->read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));
        m_vecOffsetMatrices.push_back(OffsetMatrix);
    }

    size_t BoneIndicesSize;
    _pReadBinary->read(reinterpret_cast<char*>(&BoneIndicesSize), sizeof(size_t));

    for (size_t j = 0; j < BoneIndicesSize; j++)
    {
        _uint BoneIndex;
        _pReadBinary->read(reinterpret_cast<char*>(&BoneIndex), sizeof(_uint));
        m_vecBoneIndices.push_back(BoneIndex);
    }

    //// 인덱스 정보
    _ulong* pIndices = new _ulong[iNumIndices];
    _pReadBinary->read(reinterpret_cast<char*>(pIndices), sizeof(_ulong) * iNumIndices);


#pragma endregion







#pragma region MEMBER_VARIABLE

    // Setting Member Variable
    {
        m_iMaterialIndex = iMaterialIndex;
        strcpy_s(m_szName, szName);
        m_iNumBones = iNumBones;

        // Vertex
        m_iNumVertices = iNumVertices; 
        m_iNumVertexBuffers = 1;
        m_iVertexStride = sizeof(VTXANIMMESH);

        // Index
        m_iNumIndices = iNumIndices; // mNumFaces = 삼각형 면의 개수를 의미하며, 고로 하나의 삼각형은 3개의 인덱스로 구성되어 있다
        m_iIndexStride = 4; //원래는 조건문 달아서 6만 5천개를 넘는지 않는지 하면 되지만, 쌤은 넉넉하게 4바이트로 잡으셨다
        m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

#pragma endregion
   








#pragma region VERTEX_BUFFER

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
    
#pragma endregion









#pragma region INDEX_BUFFER

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

#pragma endregion







    // Safe Release
    {
        Safe_Delete_Array(pVertices);
        Safe_Delete_Array(pIndices);
    }

    return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, std::ifstream* _pReadBinary, CModel* _pModel, CModel::TYPE _eType, _fmatrix _PivotMatrix)
{
	CMesh* pInstance = new CMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pReadBinary, _pModel, _eType, _PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* _pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

    Safe_Delete_Array(m_pVertices);
    Safe_Delete_Array(m_pIndices);
}
