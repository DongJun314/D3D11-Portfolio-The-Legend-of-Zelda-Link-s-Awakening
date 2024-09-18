#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice,_pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& _rhs)
	: CComponent(_rhs)
	, m_pVB(_rhs.m_pVB)
	, m_pIB(_rhs.m_pIB)
	, m_iVertexStride(_rhs.m_iVertexStride)
	, m_iNumVertices(_rhs.m_iNumVertices)
	, m_iIndexStride(_rhs.m_iIndexStride)
	, m_iNumIndices(_rhs.m_iNumIndices)
	, m_iNumVertexBuffers(_rhs.m_iNumVertexBuffers)
	, m_eIndexFormat(_rhs.m_eIndexFormat)
	, m_ePrimitiveTopology(_rhs.m_ePrimitiveTopology)
	, m_iNumVerticesX(_rhs.m_iNumVerticesX)
	, m_iNumVerticesY(_rhs.m_iNumVerticesY)
	, m_iNumVerticesZ(_rhs.m_iNumVerticesZ)
	, m_pVerticesPos(_rhs.m_pVerticesPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	//m_pContext->DrawIndexedInstanced();
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = { 
		m_pVB, 
	};

	_uint iVertexStrides[] = { 
		m_iVertexStride, 
	};

	_uint iOffsets[] = { 
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat,0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** _ppBuffer)
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, _ppBuffer);
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_bIsCloned)
		Safe_Delete_Array(m_pVerticesPos);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
