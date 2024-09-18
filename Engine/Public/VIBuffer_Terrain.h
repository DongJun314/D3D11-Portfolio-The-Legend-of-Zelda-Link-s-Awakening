#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& _rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& _strHeightMapFilePath);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	_float Compute_Height(_fvector _vLocalPos);
	
public:
	static CVIBuffer_Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strHeightMapFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint*			m_pIndices = {nullptr};
	VTXNORTEX*		m_pVertices = {nullptr};
};

END