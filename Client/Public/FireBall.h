#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CFireBall final : public CMonster
{
private:
	CFireBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFireBall(const CFireBall& _rhs);
	virtual ~CFireBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_ToPlayer() override;
	virtual _bool Collision_ToPlayerBody(MCOLLIDER _eType) override;
	virtual _bool Collision_ToPlayerSword(MCOLLIDER _eType) override; 
	virtual _bool Collision_ToPlayerShield(MCOLLIDER _eType) override;
	virtual void Invalidate_ColliderTransformationMatrix() override;	

private:
	HRESULT Add_Component();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();

	void Handle_Spheres_Sliding(CGameObject* _pGameObject);

public:
	static CFireBall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_vector m_vPlayerDir = { 0.f, 0.f, 0.f, 0.f };
	CCollider* m_pColliderCom = { nullptr };

	_float4x4 m_ColliderWorldMatrix;
	_float4x4* m_pSocketMatrix = { nullptr };
};

END