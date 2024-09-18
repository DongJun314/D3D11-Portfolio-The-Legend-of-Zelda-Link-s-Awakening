#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CWeapon final : public CPartObject
{
public:
	enum WEAPON_COLLIDER { WTYPE_SWORD, WTYPE_SHIELD, WTYPE_END };

public:
	typedef struct Weapon_Desc : public CPartObject::PARTOBJ_DESC
	{
		// 모델의 뼈들 중에서 특히나 다른 모델의 붙여야하는 뼈들 있다, 그걸 보통 주로 Socket Bone이라고 한다
		_float4x4* pSocketMatrix[WTYPE_END];
	}WEAPON_DESC;

private:
	CWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWeapon(const CWeapon& _rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();


public:
	static CWeapon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom[WTYPE_END] = {nullptr};



	_float4x4 m_WeaponWorldMatrix[WTYPE_END];
	_float4x4* m_pSocketMatrix[WTYPE_END] = {nullptr};
	_float4x4* m_pTransformationMatrix = { nullptr };
};

END