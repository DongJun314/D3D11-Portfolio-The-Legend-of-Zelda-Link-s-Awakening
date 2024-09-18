#include "stdafx.h"
#include "../Public/MainApp.h"

// Client
#include "Level_Loading.h"
#include "Level_Logo.h"

// Engine
#include "GameInstance.h"
#include "BackGround.h"
#include "Player.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC	GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GraphicDesc));
	{
		GraphicDesc.hWnd = g_hWnd;
		GraphicDesc.bIsWindowed = true;
		GraphicDesc.iWinSizeX = g_iWinSizeX;
		GraphicDesc.iWinSizeY = g_iWinSizeY;
	}

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Gara()))
		return E_FAIL;

	if (FAILED(Ready_Interface_Prototype()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float _fTimeDelta)
{
	m_pGameInstance->Tick_Engine(_fTimeDelta);
}
 
HRESULT CMainApp::Render()
{
	/*if (FAILED(m_pGameInstance->Draw(_float4(0.5f, 0.5f, 1.f, 1.f))))
		return E_FAIL;*/

	if (FAILED(m_pGameInstance->Begin_Draw(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("Á©´ÙÀÇ Àü¼³: ²Þ²Ù´Â ¼¶"), _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_Draw()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/Link's_Awakening_Font.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL _eStartLevel)
{
	if (LEVEL_LOADING == _eStartLevel)
		return E_FAIL;
	
	return m_pGameInstance->Open_Level(_eStartLevel, CLevel_Logo::Create(m_pDevice, m_pContext));
}

HRESULT CMainApp::Ready_Interface_Prototype()
{
	// Component ¿øÇü, ¸Ê¿¡ Ãß°¡
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
			CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Zelda_Logo.png"), 1))))
			return E_FAIL;
	}

	// GameObject ¿øÇü, ¸Ê¿¡ Ãß°¡
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
			CBackGround::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}
	
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}
