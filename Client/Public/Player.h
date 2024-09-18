#pragma once

#include "LandObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CLandObject
{
public:
	enum Coll_Monster { BLUEMORIBLIN, ZOMBIE, STALFOSORANGE, POTDEMON, DEMONKING_L, DEMONKING_R, CUCCO, BOMBERGREEN, CHAINIRONBALL, RANSENGAN, CM_END };
private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& _rhs);
	virtual ~CPlayer() = default;
	
public:
	void Set_FirstView(_uint _bView) {
		m_bFirstView = _bView;
	};

	void Set_MiniGameA(_uint _bScene) {
		m_bMiniGameA = _bScene;
	};
	void Set_IsBossStage(_uint _bScene) {
		m_bIsBossStage = _bScene;
	};

	void Set_CurState(_uint _iCurState) {
		m_iState = _iCurState;
	}
	void Set_CollMonster(_uint _iCollMonster) {
		m_iCollMonster = _iCollMonster;
	}

	_bool Get_FirstView() const {
		return m_bFirstView;
	};

	_bool Get_MiniGameA() const {
		return m_bMiniGameA;
	}
	_bool Get_IsBossStage() const {
		return m_bIsBossStage;
	}

	_uint Get_CurState() const {
		return m_iState;
	}

	_bool Get_JumpState() const {
		return m_bIsUpJumped;
	}

	_bool Get_UltJumpState() const {
		return m_bUltJump;
	}

	_bool Get_UltSTState() const {
		return m_bUltST;
	}

	_bool Get_UltLPState() const {
		return m_bUltLP;
	}

	_bool Get_UltEDState() const {
		return m_bUltED;
	}

	_int Get_FireBallCloneCount() const {
		return m_iFireBallCloneCount;
	}

public:
	class CComponent* Get_PartObjectComponent(const wstring& _strPartObjTag, const wstring& _strComTag);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void MainGame_KeyInput(_float _fTimeDelta);
	void MiniGameA_KeyInput(_float _fTimeDelta);
	void BossStage_KeyInput(_float _fTimeDelta);
	void FirstView_KeyInput(_float _fTimeDelta);
	void FirstView_MiniGameA_KeyInput(_float _fTimeDelta);
	void FirstView_BossStage_KeyInput(_float _fTimeDelta);
	void Throw_FireBalls(_float _fTimeDelta);
	void SetUp_OnGames_Terrain();

public:
	void PlaySound();

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

public:
	static CPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	// Mini GameA & B
	_bool	m_bFirstView = false;
	_bool	m_bMiniGameA = false;
	_bool	m_bIsBossStage = false;
	_bool	m_bFireBallSound = false;

	_int m_iFireBallCloneCount = { -1 };
	_bool m_bIsFinishedShooting = false;
	_bool m_bIsSpecialSlash = false;
	_float m_fFireBallTime = { 0.f };

	_uint	m_iState = { 0 };
	_uint	m_iPlayerDir = { 0 };
	_uint	m_iCollMonster = { 0 };
	_float	m_fChargingGauge = { 0.f };

	// Jumping
	_float	m_fJumpSpeed = { 0.2f };
	_float	m_fJumpTime = { 0.f };
	_bool	m_bIsUpJumped = false;
	_bool	m_bIsDownJumped = false;
	_bool	m_bIsLeftJumped = false;
	_bool	m_bIsRightJumped = false;

	// DMG_QUAKE
	_bool	m_bGetOverlapOnce = false;
	_float  m_fOverLap = 0.f;

	_bool	m_bIsTakingDamage = false;
	_bool	m_bIsInvincible = false;
	_float	m_fInvincibleDuration = { 0.3f };
	_float	m_fInvincibleTimer = { 0.f };
	_bool	m_bMoveOnce = false;
	_bool	m_bGetPosOnce = false;
	_float  m_fDmgQuake = { 0.05f };
	_vector m_vPlayerPos = { 0.f,0.f,0.f,0.f };
	_vector m_vMonsterPos = { 0.f,0.f,0.f,0.f };

	// DMG_BOMB
	_float m_fDmgBomb = { 0.3f };

	// Ult
	_bool m_bUltJump = false;
	_bool m_bCancelOnTerrain = false;
	_bool m_bUltST = false;	
	_bool m_bUltLP = false;
	_bool m_bUltED = false;
	_float m_fCellHeight = { 0.f };
	_float m_fUltJumpSpeed = { 3.5f };
	_float3 m_vOriginalLook = { 0.f ,0.f,0.f};

	// Sound
	_bool m_bShieldSound = false;
	_bool m_bSlashSound = false;
	_bool m_bSlashEDSound = false;
	_bool m_bUltSound = false;
	_bool m_bUltLandingSound = false;
	_bool m_bJumpSound = false;

	_float4x4* m_pParticle_SocketMatrix;
	_bool  m_bCharging = false;
	_bool m_bChargingParticle = false;
	_bool m_bLockPos = false;
	_bool m_bRotateOnce = false;
	_vector m_vLockPos = { 0.f, 0.f, 0.f, 0.f };
	map<const wstring, class CPartObject*> m_mapPlayerParts;

private:
	class CParticle_Point* m_pParticlePoint = { nullptr };
	class CEffect_Texture* m_pEffectLanding = { nullptr };
	class CEffect_Texture* m_pEffectSlash = { nullptr };

	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };

	// For MiniGameA & B
	CNavigation* m_pMiniGameA_NavigationCom = { nullptr };
	CNavigation* m_pBossStage_NavigationCom = { nullptr };
};

END