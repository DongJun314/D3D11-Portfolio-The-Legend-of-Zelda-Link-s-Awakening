#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CZombie final : public CMonster
{
	enum ZOMBIE { ZTYPE_BODY, ZTYPE_BACK, ZTYPE_END };
private:
	CZombie(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZombie(const CZombie& _rhs);
	virtual ~CZombie() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Collision_ToPlayer() override;
	_bool Collision_ToPlayerBody(ZOMBIE _eType);
	_bool Collision_ToPlayerSword(ZOMBIE _eType);
	_bool Collision_ToPlayerShield(ZOMBIE _eType);
	virtual void Invalidate_ColliderTransformationMatrix() override;

private:
	HRESULT Add_Component();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();

	void Handle_Spheres_Sliding(CGameObject* _pGameObject);

public:
	static CZombie* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	class CEffect_Texture* m_pEffect_Blood = { nullptr };

private:
	CCollider* m_pColliderCom[ZTYPE_END] = { nullptr };

	_float4x4 m_ColliderWorldMatrix[ZTYPE_END];
	_float4x4* m_pSocketMatrix[ZTYPE_END] = { nullptr };
};

END