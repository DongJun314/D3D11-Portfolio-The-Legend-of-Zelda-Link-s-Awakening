#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte	Get_DIKeyState(_ubyte _byKeyID) { 
		return m_byKeyState[_byKeyID]; 
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE _eMouse) { 
		return m_tMouseState.rgbButtons[_eMouse]; 
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE _eMouseState){
		return *(((_long*)&m_tMouseState) + _eMouseState);
	}

public:
	HRESULT Ready_InputDev(HINSTANCE _hInst, HWND _hWnd);
	void	Tick();

public:
	static CInput_Device* Create(HINSTANCE _hInst, HWND _hWnd);
	virtual void	Free();

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_byte					m_byKeyState[256] = { 0 };		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState = { 0 };

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;

};
END
#endif // InputDev_h__

