#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& _rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* _pPoints);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	_float Compute_Height(_fvector _vLocalPos);

public:
	static CVIBuffer_Cell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
};

END