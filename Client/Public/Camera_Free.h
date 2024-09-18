#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

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
	void Set_Player_PartsObject(class CPartObject* _pPartsObject) {
		m_pPlayerFirstView = _pPartsObject;
	};
	void Shake_Camera(_float _fTimeDelta, _float _fShakingPower, _float _fShakingTime);

public:
	static CCamera_Free* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_bool m_bLock = false;
	_bool m_bLock2 = false;
	_float			m_fMouseSensor = { 0.0f };
	//_float			m_fCameraShaking = { 1.0f };

	///Test
	_uint	m_iNumShakingCount = { 0 };
	int		m_iShakingPower = { 0 };
	_float	m_fShakingTime = { 0.f };

	class CPartObject* m_pPlayerFirstView = { nullptr };
};

END

