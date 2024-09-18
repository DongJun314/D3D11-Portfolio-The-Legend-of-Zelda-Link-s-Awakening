#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct Sphere_Desc : public CBounding::BOUNDING_DESC
	{
		_float fRadius;
	}SPHERE_DESC;

private:
	CBounding_Sphere(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pSphere;
	}

public:
	HRESULT Initialize(void* _pArg);
	virtual void Tick(_fmatrix _WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* _pBatch) override;

	_bool Intersect(CCollider::TYPE _eType, CBounding* _pBounding);

public:
	static CBounding_Sphere* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;

private:	
	BoundingSphere* m_pOriginalSphere = { nullptr };
	BoundingSphere* m_pSphere = { nullptr };
};

END