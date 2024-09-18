#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CBonePutter final : public CMonster
{
private:
	CBonePutter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBonePutter(const CBonePutter& _rhs);
	virtual ~CBonePutter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Collision_ToFireBall();
	_bool Collision_ToFireBallCollider();
	virtual void Invalidate_ColliderTransformationMatrix() override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CBonePutter* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	CCollider* m_pColliderCom = { nullptr };

	_float4x4 m_ColliderWorldMatrix[TYPE_END];
	_float4x4* m_pSocketMatrix[TYPE_END] = { nullptr };
};

END
