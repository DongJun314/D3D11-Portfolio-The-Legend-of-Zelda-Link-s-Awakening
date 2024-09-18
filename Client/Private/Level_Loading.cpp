#include "stdafx.h"
#include "Level_Loading.h"

// Client
#include "Loader.h"
#include "BackGround.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"

// Engine
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	Ready_Layer_BackGround(TEXT("Layer_BackGround_Loading"));

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, _eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float _fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		//if (GetKeyState(VK_SPACE) & 0x8000)
		//{
			CLevel* pLevel = { nullptr };

			switch (m_eNextLevel)
			{
			case LEVEL_GAMEPLAY:
				pLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
				break;

				//추후 관련 레벨 추가
		
			}

			if (nullptr == pLevel)
				return;
		
			if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel)))
				return;


		//}
	}
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pLoader->Output_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const wstring& _strLayerTag)
{
	CBackGround::BACKGROUND_DESC BackGroundDesc;
	ZeroMemory(&BackGroundDesc, sizeof(BackGroundDesc));
	{
		BackGroundDesc.iBackData = 10;
		BackGroundDesc.iGameObjectData = 10;
		BackGroundDesc.fSpeedPerSec = 10.f;
		BackGroundDesc.fRotationPerSec = XMConvertToRadians(60.f);
	}

	// Component 원형, 맵에 추가
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/Zelda_Loading.png"), 1))))
		return E_FAIL;
	}

	
	// GameObject 원형, 맵에 추가
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOADING, _strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL _eNextLevel)
{
	CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}