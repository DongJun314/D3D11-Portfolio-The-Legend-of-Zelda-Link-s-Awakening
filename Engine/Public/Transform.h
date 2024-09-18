#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum ROTATION { TO_FORWARD, TO_RIGHT, TO_LEFT, TO_BACKWARD, TO_FRIGHT, TO_FLEFT, TO_BRIGHT, TO_BLEFT, TO_END };
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	typedef struct Transform_Desc
	{
		_float fSpeedPerSec = { 0.f };
		_float fRotationPerSec = { 0.f };
		_float4x4 WorldMatrix = {};
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;

public:
	void Set_RotationPerSec(_float _fRotationPerSec) {
		m_fRotationPerSec = _fRotationPerSec;
	}
	void Set_SpeedPerSec(_float _fSpeedPerSec) {
		m_fSpeedPerSec = _fSpeedPerSec;
	}

	_float3 Get_Scaled()
	{
		return _float3(
			XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
			XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]
		);
	}
	void Set_Scaling(_float _fX, _float _fY, _float _fZ);

	_vector Get_State(STATE _eState)
	{
		// Load는 읽어오겠다, Store는 저장하겠다, r = raw(행)
		return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
	}

	void Set_State(STATE _eState, _fvector _vState)
	{
		_float4 vTmp;
		XMStoreFloat4(&vTmp, _vState);
		memcpy(&m_WorldMatrix.m[_eState], &vTmp, sizeof(_float4));
	}

	void Set_WorldMatrix(_matrix _WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, _WorldMatrix);
	}

	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

public:
	HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName);

public:
	void Go_Straight(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Backward(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Left(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Right(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Go_Up(_float _fTimeDelta);
	void Go_Down(_float _fTimeDelta);
	_bool Go_PosLerp(_float _fTimeDelta, _vector _vTargetPos, _float _fVecOffset, _vector _vDistance);

	void Turn(_fvector _vAxis, _float _fTimeDelta);
	void TurnTo(ROTATION _eRotation, _float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
	void Rotation(_fvector _vAxis, _float _fRadian);
	void MoveTo(_fvector _vPoint, _float _fLimit, _float _fTimeDelta);
	void LookAt(_fvector _vPoint);
	void LookAt_ForLandObject(_fvector _vPoint);

public:
	static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float m_fSpeedPerSec = { 0.0f };
	_float m_fRotationPerSec = { 0.0f };

	_float4x4				m_WorldMatrix = {};
	TRANSFORM_DESC			m_TransformDesc;
};

END