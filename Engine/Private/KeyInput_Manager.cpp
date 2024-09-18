#include "KeyInput_Manager.h"

CKeyInput_Manager::CKeyInput_Manager()
{
}

HRESULT CKeyInput_Manager::Initialize()
{
    return S_OK;
}

_bool CKeyInput_Manager::Key_Up(_int _iKey)
{
    if (m_bKeyStateUp[_iKey] && !(GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyStateUp[_iKey] = !m_bKeyStateUp[_iKey];
        return true;
    }

    for (int i = 0; i < VK_MAX; ++i)
    {
        if (!m_bKeyStateUp[i] && (GetAsyncKeyState(i) & 0x8000))
            m_bKeyStateUp[i] = !m_bKeyStateUp[i];
    }

    return false;
}

_bool CKeyInput_Manager::Key_Down(_int _iKey)
{
    if ((!m_bKeyStateDown[_iKey]) && (GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyStateDown[_iKey] = !m_bKeyStateDown[_iKey];
        return true;
    }

    for (int i = 0; i < VK_MAX; ++i)
    {
        if (m_bKeyStateDown[i] && !(GetAsyncKeyState(i) & 0x8000))
            m_bKeyStateDown[i] = !m_bKeyStateDown[i];
    }

    return false;
}

_bool CKeyInput_Manager::Key_Pressing(_int _iKey)
{
    if (GetAsyncKeyState(_iKey) & 0x8000)
        return true;

    return false;
}

CKeyInput_Manager* CKeyInput_Manager::Create()
{
    CKeyInput_Manager* pInstance = new CKeyInput_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CKeyInput_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKeyInput_Manager::Free()
{
}
