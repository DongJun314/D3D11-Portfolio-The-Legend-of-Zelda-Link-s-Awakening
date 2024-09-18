#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct
	{
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBB_COL_DESC;

public:
	typedef struct OBB_Desc : public CBounding::BOUNDING_DESC
	{
		_float3 vExtents;
		_float3 vRadians; /* x : x축 기준 회전 각도, y : y축 기준 회전 각도, z : z축 기준 회전 각도 */
	}OBB_DESC;

private:
	CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding_OBB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pOBB;
	}

public:
	HRESULT Initialize(void* _pArg);
	virtual void Tick(_fmatrix _WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* _pBounding) override;

public:
	OBB_COL_DESC Compute_OBBColDesc();

private:
	_bool Intersect(CBounding_OBB* _pTargetBounding);

public:
	static CBounding_OBB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override; 



private: 
	BoundingOrientedBox* m_pOriginalOBB = { nullptr };
	BoundingOrientedBox* m_pOBB = { nullptr };
};

END