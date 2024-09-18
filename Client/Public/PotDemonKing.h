#pragma once

#include "PotDemonPartObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CPotDemonKing final : public CPotDemonPartObject
{
public:
	enum DEMON_KING_COLLIDER { DKC_ROOT, DKC_HAND_L, DKC_HAND_R, DKC_FB_A, DKC_FB_B, DKC_FB_C, DKC_END };

public:
	typedef struct Demon_King_Desc : public CPotDemonPartObject::POTDEMON_PARTOBJ_DESC
	{
		_bool* bPhase2Pop;
		_bool* bPhase2Wait;
		_bool* bPhase2Disappear;
		_bool* bPhase2Appear;
		_bool* bPhase2Punch;
		_bool* bPhase2PunchED;
		_bool* bPhase2ThrowST;
		_bool* bPhase2ThrowLP;
		_bool* bPhase2ThrowED;
		_bool* bPhase2Damage;
		_bool* bPhase2Dead;

		_int* iDemonKingState;

		_uint* iCucco_CloneCount;
		_uint* iChainIronBall_CloneCount;
		_uint* iBomberGreen_CloneCount;
		_uint* iRasengan_CloneCount;

		// 모델의 뼈들 중에서 특히나 다른 모델의 붙여야하는 뼈들 있다, 그걸 보통 주로 Socket Bone이라고 한다
		_float4x4* pSocketMatrix[DKC_END];

	}DEMON_KING_DESC;

private:
	CPotDemonKing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPotDemonKing(const CPotDemonKing& _rhs);
	virtual ~CPotDemonKing() = default;

public:
	_float4x4 Get_FireA_WorldMatrix() {
		return m_pFire_A_WorldMatrix;
	}

	_float4x4 Get_FireB_WorldMatrix() {
		return m_pFire_B_WorldMatrix;
	}

	_float4x4 Get_FireC_WorldMatrix() {
		return m_pFire_C_WorldMatrix;
	}

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
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();
	void Handle_Spheres_Sliding(CGameObject* _pGameObject);

public:
	static CPotDemonKing* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_float4x4 m_pHand_L_WorldMatrix;
	_float4x4 m_pHand_R_WorldMatrix;
	_float4x4 m_pFire_A_WorldMatrix;
	_float4x4 m_pFire_B_WorldMatrix;
	_float4x4 m_pFire_C_WorldMatrix;

private:
	_bool m_bThrowOnce = false;
	_bool m_bThrowTwice = false;

	_bool* m_bPhase2Pop;
	_bool* m_bPhase2Wait;
	_bool* m_bPhase2Disappear;
	_bool* m_bPhase2Appear;
	_bool* m_bPhase2Punch;
	_bool* m_bPhase2PunchED;
	_bool* m_bPhase2ThrowST;
	_bool* m_bPhase2ThrowLP;
	_bool* m_bPhase2ThrowED;
	_bool* m_bPhase2Damage;
	_bool* m_bPhase2Dead;

	_bool* m_bOnFireCucco;
	_bool* m_bOnFireBomberGreen;
	_bool* m_bOnFireChainIronBall;
	_bool* m_bOnFireRasengan;

	_int* m_iDemonKingState = { nullptr };

	_uint* m_iCucco_CloneCount;
	_uint* m_iChainIronBall_CloneCount;
	_uint* m_iBomberGreen_CloneCount;
	_uint* m_iRasengan_CloneCount;

	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom[DKC_END] = { nullptr };
	
	_float4x4* m_pSocketMatrix[DKC_END] = { nullptr };
	_float4x4* m_pTransformationMatrix = { nullptr };
};

END