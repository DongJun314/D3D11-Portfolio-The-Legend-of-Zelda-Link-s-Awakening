#pragma once

#include "Editor_Defines.h"
#include "Camera.h"

BEGIN(Editor)

class CCamera_Free final : public CCamera
{
public:
	typedef struct Camera_Free_Desc : public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.f };
	} CAMERA_FREE_DESC;

protected:
	CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Free(const CCamera_Free& _rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	_matrix Get_CamWorldMatrix() { return m_pTransformCom->Get_WorldMatrix(); }

public:
	static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	///////////////////////////////////////////////////
	////////////////* Member Variable *////////////////
	///////////////////////////////////////////////////
private:
	_bool			m_bMouseMove = false;
	_float			m_fMouseSensor = { 0.0f };
	_float			m_fTimeAcc = { 0.0f };

	int toggle = 1;
};

END

