#include "Input_Device.h"


CInput_Device::CInput_Device()
{

}

HRESULT CInput_Device::Ready_InputDev(HINSTANCE _hInst, HWND _hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	{
		if (FAILED(DirectInput8Create(_hInst,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_pInputSDK,
			nullptr)))
			return E_FAIL;
	}

	// Ű���� ��ü ����
	{
		if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
			return E_FAIL;

		// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
		m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

		// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
		m_pKeyBoard->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

		// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
		m_pKeyBoard->Acquire();
	}


	// ���콺 ��ü ����
	{
		if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
			return E_FAIL;

		// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
		m_pMouse->SetDataFormat(&c_dfDIMouse);

		// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
		m_pMouse->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

		// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
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
