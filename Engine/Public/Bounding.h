#pragma once

#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase 
{
public:
	typedef struct Bounding_Desc
	{
		_float3 vCenter;
	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding() = default;

public:
	virtual void* Get_Bounding() = 0;

public:
	virtual void Tick(_fmatrix _WorldMatrix) = 0;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) = 0;
	virtual _bool Intersect(CCollider::TYPE _eType, CBounding* _pBounding) = 0;

public:
	virtual void Free() override;


protected:
	_bool						m_bIsColl = { false };

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END