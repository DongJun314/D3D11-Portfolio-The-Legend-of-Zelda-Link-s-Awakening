#pragma once

#include "Base.h"

BEGIN(Engine)

class CKeyInput_Manager final : public CBase
{
private:
	CKeyInput_Manager();
	virtual ~CKeyInput_Manager() = default;

public:
	HRESULT Initialize();

public:
	_bool Key_Up(_int _iKey);
	_bool Key_Down(_int _iKey);
	_bool Key_Pressing(_int _iKey);

public:
	static CKeyInput_Manager* Create();
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_bool m_bKeyState[VK_MAX] = {};
	_bool m_bKeyStateDown[VK_MAX] = {};
	_bool m_bKeyStateUp[VK_MAX] = {};
};

END