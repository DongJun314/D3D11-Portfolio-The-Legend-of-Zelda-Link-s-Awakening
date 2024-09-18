#include "pch.h"
#include "EditorApp.h"

// Editor
#include "Window.h"
#include "Level_Loading.h"
#include "ImGui_Manager.h"

// Engine
#include "GameInstance.h"

CEditorApp::CEditorApp()
	: m_pGameInstance(CGameInstance::GetInstance())
    , m_pImGui_Manager(CImGui_Manager::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
   // Safe_AddRef(m_pImgui_Mgr);
}

HRESULT CEditorApp::Initialize()
{

#pragma region GRAPHIC_DESC
    GRAPHIC_DESC	GraphicDesc;
    ZeroMemory(&GraphicDesc, sizeof(GraphicDesc));
    {
        GraphicDesc.hWnd = g_hWnd;
        GraphicDesc.bIsWindowed = true;
        GraphicDesc.iWinSizeX = g_iWinSizeX;
        GraphicDesc.iWinSizeY = g_iWinSizeY;
    }
#pragma endregion

    if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    m_pWindow = CWindow::Create(m_pDevice, m_pContext);
    if (nullptr == m_pWindow)
        return E_FAIL;

    //if (FAILED(Ready_Prototype_Component_Static()))
    //    return E_FAIL;
    
    if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
        return E_FAIL;
    
    if (FAILED(m_pImGui_Manager->Initialize(m_pDevice, m_pContext, m_pGameInstance)))
        return E_FAIL;

	return S_OK;
}

void CEditorApp::Tick(_float _fTimeDelta)
{
    m_pGameInstance->Tick_Engine(_fTimeDelta);
    m_pWindow->Tick(_fTimeDelta);
}

HRESULT CEditorApp::Render()
{
    if (FAILED(m_pGameInstance->Clear_Views(m_pImGui_Manager->Get_CanvasColor())))
        return E_FAIL;
 
    m_pImGui_Manager->Render();

    if (FAILED(m_pGameInstance->Present_SwapChain()))
        return E_FAIL;

	return S_OK;
}

HRESULT CEditorApp::Open_Level(LEVEL _eStartLevel)
{
    if (LEVEL_LOADING == _eStartLevel)
        return E_FAIL;

    return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, _eStartLevel));
}

HRESULT CEditorApp::Ready_Prototype_Component_Static()
{
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    return S_OK;
}

CEditorApp* CEditorApp::Create()
{
	CEditorApp* pInstance = new CEditorApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CEditorApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditorApp::Free()
{
    Safe_Release(m_pWindow);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    Safe_Release(m_pImGui_Manager);

    //CImGui_Manager::GetInstance()->Free();

    //CImGui_Manager::GetInstance()->DestroyInstance();

    CGameInstance::Release_Engine();
}
