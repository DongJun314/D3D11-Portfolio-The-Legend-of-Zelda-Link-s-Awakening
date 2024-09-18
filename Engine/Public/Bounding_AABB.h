#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct AABB_Desc : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	}AABB_DESC;

private:
	CBounding_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pAABB;
	}

public:
	HRESULT Initialize(void* _pArg);
	virtual void Tick(_fmatrix _WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* _pBounding) override;

	_bool Intersect(BoundingBox* _pTargetAABB);

public:
	static CBounding_AABB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;


private:
	BoundingBox* m_pOriginalAABB = { nullptr };
	BoundingBox* m_pAABB = { nullptr };
};

END