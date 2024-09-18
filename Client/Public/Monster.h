#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CNavigation;
END


BEGIN(Client)

class CMonster abstract : public CLandObject
{
public:
	enum MCOLLIDER { TYPE_BODY, TYPE_SWORD, TYPE_SHIELD, TYPE_END };

protected:
	CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMonster(const CMonster& _rhs);
	virtual ~CMonster() = default;
	
public:
	_uint Get_HitCount() const {
		return m_iHitCount;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Rotate_Monster(_bool* _bState);

	virtual void Collision_ToPlayer();
	virtual _bool Collision_ToPlayerBody(MCOLLIDER _eType);
	virtual _bool Collision_ToPlayerSword(MCOLLIDER _eType);
	virtual _bool Collision_ToPlayerShield(MCOLLIDER _eType);
	virtual void Invalidate_ColliderTransformationMatrix();


	_bool Compare_DirVectors(const _vector& vec1, const _vector& vec2, _float tolerance = 0.001f);


public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	// Sound
	_bool m_bSoundOnce = false;
	_bool m_bDeadSoundOnce = false;

	_bool m_bAvoidSuddenTurn = false;
	_bool m_bFindBefore = false;
	_bool m_bFind = false;
	_bool m_bWalk = false;
	_bool m_bAttack = false;
	_bool m_bGuard = false;
	_bool m_bHit = false;
	_bool m_bStagger = false;
	_bool m_bDead = false;

	// For Zombie
	_bool m_bWait = false;
	_bool m_bDepop = false;

	_float m_fTimeDelta = { 0.f };
	_float m_fStagger = { 0.015f };
	_uint m_iHitCount = { 0 };
	_float  m_fDisToPlayer = 0.f;
	_vector m_vPlayerPos;
	_vector m_vDirToPlayer;

	_float4x4 m_WorldMatrix;

protected:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };	
	CTransform* m_pParentTransform = { nullptr };
};

END
