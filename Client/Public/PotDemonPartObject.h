#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPotDemonPartObject abstract : public CGameObject
{
public:
	typedef struct PotDemon_PartObj_Desc
	{
		_bool* bDemonKingComeOut;

		_bool* bWait;
		
		_bool* bPop;
		_bool* bAtkFire;
		_bool* bAtkJuggling;
		_bool* bAtkUlt;
		_bool* bDepop;
		_bool* bDead;

		_bool* bHit;
		_uint* iHitCount;

		_bool* bOnFireCucco;
		_bool* bOnFireBomberGreen;
		_bool* bOnFireChainIronBall;
		_bool* bOnFireRasengan;

		CTransform* pParentTransfrom;

	}POTDEMON_PARTOBJ_DESC;

protected:
	CPotDemonPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPotDemonPartObject(const CPotDemonPartObject& _rhs);
	virtual ~CPotDemonPartObject() = default;

public:
	void Set_Scaling(_float _fX, _float _fY, _float _fZ);

	void Set_State(_fvector _vState)
	{
		_float4 vTmp;
		XMStoreFloat4(&vTmp, _vState);
		memcpy(&m_WorldMatrix.m[3], &vTmp, sizeof(_float4));
	}

	_float4x4 Get_PartsWorldMatrix() const {
		return m_WorldMatrix;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;

protected:
	_bool m_bGetPosOnce = false;
	_bool m_bGetInfoOnce = false;

	_bool* m_bDemonKingComeOut;
	_bool* m_bWait;

	_bool* m_bPop;
	_bool* m_bDepop;
	_bool* m_bDead;

	_bool* m_bHit;
	_uint* m_iHitCount;

	_bool* m_bAtkUlt;
	_bool* m_bAtkFire;
	_bool* m_bAtkJuggling;
	


	_float4x4 m_WorldMatrix;
	CTransform* m_pParentTransform = { nullptr };
};

END