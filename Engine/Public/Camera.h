#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject 
{
public:
	typedef struct Camera_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4		vEye, vAt;
		_float		fFovY = { 0.0f };
		_float		fNear = { 0.0f };
		_float		fFar = { 0.0f };
		_float		fAspect = { 0.0f };

	}CAMERA_DESC;

public:
	CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera(const CCamera& _rhs);
	virtual ~CCamera() = default;

public:
	void Set_FovY(_float _fFovY) {
		m_fFovY = _fFovY;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_TransformMatices();

public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	_float m_fFovY = { 0.f };
	_float m_fNear = { 0.f };
	_float m_fFar = { 0.f };
	_float m_fAspect = { 0.f };
};

END