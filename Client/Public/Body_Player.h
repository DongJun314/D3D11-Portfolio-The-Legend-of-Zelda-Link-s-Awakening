#pragma once

#include "PartObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	typedef struct Body_Player_Desc : public CPartObject::PARTOBJ_DESC
	{
		_uint* pPlayerState;
		_bool* bUltJump;
		_bool* bUltST;
		_bool* bUltLP;
		_bool* bUltED;
		_bool* bIsSpecialSlash;
		_bool* bCancelOnTerrain;
		_bool  bChargingGauge;
		
	}BODY_PLAYER_DESC;

private:
	CBody_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBody_Player(const CBody_Player& _rhs);
	virtual ~CBody_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Invalidate_ColliderTransformationMatrix();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_bool			m_bChargingGauge = { false };
	_bool			m_bRender = { false };

	_float			m_fHitTime = { 0.f };
	_float			m_fTimeDelta = { 0.f };
	_float			m_fRenderTime = { 0.1f };


private:
	_bool*			m_bCancelOnTerrain;
	_bool*			m_bUltJump;
	_bool*			m_bUltST;
	_bool*			m_bUltLP;
	_bool*			m_bUltED;
	_bool*			m_bIsSpecialSlash;
	_uint*			m_pPlayerState;
	
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
};

END
