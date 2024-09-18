#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Particle_Rect final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Particle_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& _rhs);
	virtual ~CVIBuffer_Particle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint _iNumInstance);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	static CVIBuffer_Particle_Rect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumInstance);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END