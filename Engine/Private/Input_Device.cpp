#include "Input_Device.h"


CInput_Device::CInput_Device()
{

}

HRESULT CInput_Device::Ready_InputDev(HINSTANCE _hInst, HWND _hWnd)
{
	// DInput 컴객체를 생성하는 함수
	{
		if (FAILED(DirectInput8Create(_hInst,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInputSDK,
			nullptr)))
			return E_FAIL;
	}

	// 키보드 객체 생성
	{
		if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
			return E_FAIL;

		// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
		m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

		// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
		m_pKeyBoard->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

		// 장치에 대한 access 버전을 받아오는 함수
		m_pKeyBoard->Acquire();
	}


	// 마우스 객체 생성
	{
		if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
			return E_FAIL;

		// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
		m_pMouse->SetDataFormat(&c_dfDIMouse);

		// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
		m_pMouse->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

		// 장치에 대한 access 버전을 받아오는 함수
		m_pMouse->Acquire();
	}

	return S_OK;
}

void CInput_Device::Tick()
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

CInput_Device* CInput_Device::Create(HINSTANCE _hInst, HWND _hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Ready_InputDev(_hInst, _hWnd)))
	{
		MSG_BOX("Failed to Created : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
