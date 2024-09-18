#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CPotDemon final: public CMonster
{
private:
	CPotDemon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPotDemon(const CPotDemon& _rhs);
	virtual ~CPotDemon() = default;

public:
	class CComponent* Get_PartObjectComponent(const wstring& _strPartObjTag, const wstring& _strComTag);
	class CGameObject* Get_PartObject(const wstring& _strPartObjTag);
	void Delete_PartObject(const wstring& _strPartObjTag);
	wstring Get_BomberGreenCloneCount() const {
		return m_wstrCloneCount;
	};

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

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	void Handle_Spheres_Sliding(CGameObject* _pGameObject);

public:
	static CPotDemon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	//
	wstring m_wstrCloneCount;
	_float4x4* m_DemonKingWorldMatrix;
	_bool m_bGetUpSoundOnce = false;
	_bool m_bGetHitSoundOnce = false;
	_bool m_bPlayDeadSoundOnce = false;
	_bool m_bPlayerBGMOnce = false;

	// Pot Demon
	_bool m_bGetUp = false;
	// _bool m_bWait =false // 부모 몬스터에 있음
	_bool m_bLayDown = false;
	_bool m_bPop = false;
	_bool m_bAtkJuggling = false;
	_bool m_bAtkFire = false;
	_bool m_bAtkUlt = false;
	_bool m_bDepop = false;
	_bool m_bDemonKingComeOut = false;
	_int m_iPotDemonState = { -1 };

	// Demon King
	_bool m_bPhase2Pop = false;
	_bool m_bPhase2Wait = false;
	_bool m_bPhase2Disappear = false;
	_bool m_bPhase2Appear = false;
	_bool m_bPhase2Punch = false;
	_bool m_bPhase2PunchED = false;
	_bool m_bPhase2ThrowST = false;
	_bool m_bPhase2ThrowLP = false;
	_bool m_bPhase2ThrowED = false;
	_bool m_bPhase2Damage = false;
	_bool m_bPhase2Dead = false;

	_int m_iDemonKingState = { -1 };


	// Balls
	_bool m_bOnFireCucco = false;
	_bool m_bOnFireBomberGreen = false;
	_bool m_bOnFireChainIronBall = false;

	_int m_iCuccoState = { -1 };
	_int m_iChainIronBallState = { -1 };
	_int m_iBomberGreenState = { -1 };

	_uint m_iCucco_CloneCount = { 0 };
	_uint m_iChainIronBall_CloneCount = { 0 };
	_uint m_iBomberGreen_CloneCount = { 0 };

	// Rasengan
	_bool m_bOnFireRasengan = false;
	_int m_iRasenganState = { -1 };
	_uint m_iRasengan_CloneCount = { 0 };
	
	
	map<const wstring, class CPotDemonPartObject*> m_mapMonsterParts;

	CCollider* m_pColliderCom = { nullptr };
};

END
