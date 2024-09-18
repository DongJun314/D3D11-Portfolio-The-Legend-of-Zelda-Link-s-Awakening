#pragma once

#include "GameObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CCollider_BossToMain final : public CGameObject
{
private:
	CCollider_BossToMain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollider_BossToMain(const CCollider_BossToMain& _rhs);
	virtual ~CCollider_BossToMain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Collision_ToPlayer();
	_bool Collision_ToPlayerBody();

private:
	HRESULT Add_Component();

public:
	static CCollider_BossToMain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_bool* m_bMainScene;
	_bool* m_bMiniGameA;
	_bool* m_bMiniGameB;

	CCollider* m_pColliderCom = { nullptr };
};

END