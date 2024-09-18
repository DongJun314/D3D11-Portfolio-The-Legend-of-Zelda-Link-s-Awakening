#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CStalfosOrange final : public CMonster
{
private:
	CStalfosOrange(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CStalfosOrange(const CStalfosOrange& _rhs);
	virtual ~CStalfosOrange() = default;

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
	static CStalfosOrange* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	CCollider* m_pColliderCom[TYPE_END] = { nullptr };

	_float4x4 m_ColliderWorldMatrix[TYPE_END];
	_float4x4* m_pSocketMatrix[TYPE_END] = { nullptr };
};

END