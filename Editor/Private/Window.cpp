#include "pch.h"
#include "Window.h"

// Editor
#include "Left_Window.h"
#include "Right_Window.h"
#include "ImGui_Manager.h"

// Engine
#include "GameInstance.h"

CWindow::CWindow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWindow::Initialize()
{
	// 좌측 창을 생성합니다
	m_pLeftWindow = CLeft_Window::Create(m_pDevice, m_pContext);
	if (nullptr == m_pLeftWindow)
		return E_FAIL;

	// 우측 창을 생성합니다
	m_pRightWindow = CRight_Window::Create(m_pDevice, m_pContext);
	if (nullptr == m_pRightWindow)
		return E_FAIL;


	return S_OK;
}

void CWindow::Tick(_float _fTimeDelta)
{
	CImGui_Manager::GetInstance()->BeginTick();
	m_pLeftWindow->Tick(_fTimeDelta);
	m_pRightWindow->Tick(_fTimeDelta);
	CImGui_Manager::GetInstance()->Tick(_fTimeDelta);
	CImGui_Manager::GetInstance()->EndTick();
}

HRESULT CWindow::Render()
{
	if(FAILED(m_pLeftWindow->Render()))
		return E_FAIL;

	if (FAILED(m_pRightWindow->Render()))
		return E_FAIL;

	return S_OK;
}

CWindow* CWindow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWindow* pInstance = new  CWindow(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CWindow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow::Free()
{
	Safe_Release(m_pRightWindow);
	Safe_Release(m_pLeftWindow);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
