#pragma once

#include "PotDemonPartObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CCell;
class CModel;
class CShader;
class CCollider;
class CTransform;
class CNavigation;
END

BEGIN(Client)

class CPotDemonPot final : public CPotDemonPartObject
{

public:
	typedef struct Pot_Desc : public CPotDemonPartObject::POTDEMON_PARTOBJ_DESC
	{
		_bool* bGetUp;
		_bool* bLayDown;

		_int* pPotDemonState;
		vector<CCell*> vecCells;
		CTransform* pNavTranform;
		CNavigation* pNavigationCom;
	}POT_DESC;

private:
	CPotDemonPot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPotDemonPot(const CPotDemonPot& _rhs);
	virtual ~CPotDemonPot() = default;

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
	_bool Collision_ToPlayerSword();
	_bool Collision_ToPlayerShield();
	void Invalidate_ColliderTransformationMatrix();

public:
	HRESULT SetUp_OnTerrain(CTransform* _pTargetTransform, class CNavigation* _pNavigation);

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CPotDemonPot* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	class CEffect_Texture* m_pEffect_Pot = { nullptr };

private:
	_bool* m_bGetUp;
	_bool* m_bLayDown;

	_int* m_pPotDemonState = { nullptr };

	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_float4x4 m_ColliderWorldMatrix;
	_float4x4* m_pSocketMatrix = { nullptr };
	
	// 받아온 네비
	vector<CCell*> m_vecCells;
	CTransform* m_pNavTransform;
	CNavigation* m_pNavigationCom; 

};

END
