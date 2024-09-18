#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer(const CVIBuffer& _rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render();

public:
	HRESULT Bind_Buffers();

protected:
	HRESULT Create_Buffer(ID3D11Buffer** _ppBuffer);

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	_uint						m_iNumVertices = { 0 };
	_uint						m_iVertexStride = { 0 };
	_uint						m_iNumVertexBuffers = { 0 };
	_uint						m_iNumIndices = { 0 };
	_uint						m_iIndexStride = { 0 };

	_uint						m_iNumVerticesX = { 0 };
	_uint						m_iNumVerticesY = { 0 };
	_uint						m_iNumVerticesZ = { 0 };

	D3D11_BUFFER_DESC			m_BufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InitialData = {};
	D3D11_PRIMITIVE_TOPOLOGY    m_ePrimitiveTopology = {};
	DXGI_FORMAT					m_eIndexFormat = {};

protected:
	_float3*			m_pVerticesPos = { nullptr };
	ID3D11Buffer*		m_pVB = { nullptr };
	ID3D11Buffer*		m_pIB = { nullptr };
};

END