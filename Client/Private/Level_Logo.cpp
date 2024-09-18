#include "stdafx.h"
#include "Level_Logo.h"

// Client
#include "BackGround.h"
#include "Level_Loading.h"

// Engine
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround_Logo"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Tick(_float _fTimeDelta)
{
	if (!m_bPlayBGMOnce)
	{
		m_pGameInstance->StopSound(SOUNDTYPE::BGM);
		m_pGameInstance->PlayBGMLoop("0_Title (No Intro Version).mp3", 0.3f);
		m_bPlayBGMOnce = true;
	}
	
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (!m_bPlaySystemSoundOnce)
		{
			m_pGameInstance->PlaySoundEx("System_Click.wav", SOUNDTYPE::CHARGE, 0.4f);
			m_pGameInstance->StopSound(SOUNDTYPE::BGM);
			m_bPlaySystemSoundOnce = true;
		}

		if(FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING,CLevel_Loading::Create(m_pDevice,m_pContext,LEVEL_GAMEPLAY))))
			return;

		return;
	}
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& _strLayerTag)
{
	CBackGround::BACKGROUND_DESC BackGroundDesc;
	ZeroMemory(&BackGroundDesc, sizeof(BackGroundDesc));
	{
		BackGroundDesc.iBackData = 10;
		BackGroundDesc.iGameObjectData = 10;
		BackGroundDesc.fSpeedPerSec = 10.f;
		BackGroundDesc.fRotationPerSec = XMConvertToRadians(60.f);
	}

	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_LOGO, _strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();

}