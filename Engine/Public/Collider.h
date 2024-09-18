#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };

private:
	CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider(const CCollider& _rhs);
	virtual ~CCollider() = default;

public:
	class CBounding* Get_Bounding() const {
		return m_pBounding;
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE _eType);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	void  Tick(_fmatrix _WorldMatrix);
	HRESULT Render();

public:
	_bool Intersect(CCollider* _pTargetCollier);

public:
	static CCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eType);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;



private:
	TYPE m_eType = { TYPE_END };

private:
	class CBounding*		m_pBounding = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect* m_pEffect = { nullptr };
	ID3D11InputLayout* m_pInputLayout = { nullptr };
#endif
};

END