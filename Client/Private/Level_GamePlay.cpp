#include "stdafx.h"
#include "Level_GamePlay.h"

// Client
#include "NavMesh.h"
#include "LandObject.h"
#include "Camera_Free.h"
#include "Effect_Texture.h"

// Engine
#include "Transform.h"
#include "GameObject.h"

#include "Navigation.h"

// For Reading Binary Files
#include <fstream>

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Map_OBJ()))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_SkdyBox(TEXT("Layer_SkyBox"))))
		return E_FAIL;

	if (FAILED(Ready_Map_Nav(TEXT("Layer_Nav"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_MiniGame(TEXT("Layer_MiniGame"))))
		return E_FAIL;

	if (FAILED(Ready_Map_Boss(TEXT("Layer_Boss_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float _fTimeDelta)
{
	if (!m_bPlayBGMOnce)
	{
		m_pGameInstance->StopSound(SOUNDTYPE::BGM);
		m_pGameInstance->PlayBGMLoop("1-08 Level 7 - Eagle's Tower.mp3", 0.4f);
		m_bPlayBGMOnce = true;
	}
	
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef  _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이 레벨입니다"));
#endif //  _DEBUG

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof(LightDesc));
	{
		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	}

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& _strLayerTag)
{
	// CAMERA_FREE_DESC로 객체를 만든 이유는 초기 값이 동적으로 변할 수 있기에, 왜? 추정이유는 카메라를 여러대 만들 경우, 그 해당 포지션이 다 달라야해서?
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));
	{
		CameraDesc.fMouseSensor = 0.05f;
		CameraDesc.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
		CameraDesc.vAt = _float4(0.0f, 0.0f, 0.f, 1.f);
		//CameraDesc.fFovY = XMConvertToRadians(60.0f);
		CameraDesc.fFovY = XMConvertToRadians(90.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.001f;
		CameraDesc.fFar = 1000.f;
		CameraDesc.fSpeedPerSec = 5.f;// original
		//CameraDesc.fSpeedPerSec = 100.f; // navi용
		CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkdyBox(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MiniGame(const wstring& _strLayerTag)
{
	// Map A
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Map_GameA"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_MiniGameA"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_MiniGameA_Back"))))
		return E_FAIL;

	//
	//// Map B
	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Map_GameB"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_MiniGameB"))))
	//	return E_FAIL;
		
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& _strLayerTag, CLandObject::LANDOBJ_DESC& _LandObjDesc)
{
	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Player"), &_LandObjDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& _strLayerTag, CLandObject::LANDOBJ_DESC& _LandObjDesc)
{
	/* For Normal Monster */
	{
	/*	for (size_t i = 0; i < 20; i++)
		{*/
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Monster_BlueMoriblin"), &_LandObjDesc)))
				return E_FAIL;
		//}

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Monster_Zombie"), &_LandObjDesc)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Monster_StalfosOrange"), &_LandObjDesc)))
			return E_FAIL;
	}

	/* For Boss Monster */
	{
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_PotDemon"), &_LandObjDesc)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map_Nav(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Nav"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map_OBJ()
{	
	std::ifstream ReadBinary(TEXT("../Bin/Resources/Binary/NonAnim/Map_NonAnim.bin"), ios::in | ios::binary);

	if (!ReadBinary.is_open())
	{
		MSG_BOX("Failed to Read : Map Object Binary File");
		return E_FAIL;
	}

	size_t iCount;
	// 메시 개수
	ReadBinary.read(reinterpret_cast<char*>(&iCount), sizeof(size_t));

	for (size_t i = 0; i < iCount; i++)
	{
		size_t Length;
		ReadBinary.read(reinterpret_cast<char*>(&Length), sizeof(size_t));

		char pSzName[MAX_PATH]{};
		ReadBinary.read(reinterpret_cast<char*>(&pSzName), Length);
		string strFileName = pSzName;

		_float4x4 WorldMatrix;
		ReadBinary.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));


		// "_1, _2..." 같은 숫자 포함
		wstring wstrUniqueKey;
		wstrUniqueKey.assign(strFileName.begin(), strFileName.end());


		// "_1, _2..." 같은 숫자를 없애기 위한 과정
		size_t first = strFileName.find_last_of('_');
		size_t last = strFileName.find_last_of(strFileName);
		strFileName = strFileName.substr(0, first );

		// string을 wstring으로 바꿉니다
		wstring wstrFileName;
		wstrFileName.assign(strFileName.begin(), strFileName.end());



		// 원형 컴포넌트를 컴포넌트 매니저의 맵에 추가합니다 
		if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Object_" + wstrUniqueKey),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Binary/NonAnim/" + wstrFileName + TEXT(".bin"))))))
			return E_FAIL;

		// 원형객체의 사본을 만들고, Layer에 저장해 관리합니다
		if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_GameObject_MapOBJ"), &wstrUniqueKey)))
			return E_FAIL;

		CGameObject* pGameObject = m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"));

		_fvector vRight = { WorldMatrix._11, WorldMatrix._12, WorldMatrix._13, WorldMatrix._14 };
		_fvector vUp = { WorldMatrix._21, WorldMatrix._22, WorldMatrix._23, WorldMatrix._24 };
		_fvector vLook = { WorldMatrix._31, WorldMatrix._32, WorldMatrix._33, WorldMatrix._34 };
		_fvector vPos = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, WorldMatrix._44 };

		// 이다음에 저장해둔 월드 행렬 던져주기
		dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Set_State(CTransform::STATE_RIGHT, vRight);
		dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Set_State(CTransform::STATE_UP, vUp);
		dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Set_State(CTransform::STATE_LOOK, vLook);
		dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Set_State(CTransform::STATE_POSITION, vPos);
	}

	ReadBinary.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map_Boss(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Boss_Map"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Collider_MainToBoss"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Collider_BossToMain"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& _strLayerTag)
{
	// Blood
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture_Desc = {};
	ZeroMemory(&eEffectTexture_Desc, sizeof(eEffectTexture_Desc));
	{
		eEffectTexture_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_HIT_BLOOD;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture_Desc)))
		return E_FAIL;

	// Metal
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture1_Desc = {};
	ZeroMemory(&eEffectTexture1_Desc, sizeof(eEffectTexture1_Desc));
	{
		eEffectTexture1_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_HIT_METAL;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture1_Desc)))
		return E_FAIL;

	// Slash
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture2_Desc = {};
	ZeroMemory(&eEffectTexture2_Desc, sizeof(eEffectTexture2_Desc));
	{
		eEffectTexture2_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_SLASH_ED;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture2_Desc)))
		return E_FAIL;

	// Landing
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture3_Desc = {};
	ZeroMemory(&eEffectTexture3_Desc, sizeof(eEffectTexture2_Desc));
	{
		eEffectTexture3_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_ULT_LAND;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture3_Desc)))
		return E_FAIL;

	// Explosion
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture4_Desc = {};
	ZeroMemory(&eEffectTexture4_Desc, sizeof(eEffectTexture4_Desc));
	{
		eEffectTexture4_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_EXPLOSION;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture4_Desc)))
		return E_FAIL;

	// Particle
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Particle_Point"))))
		return E_FAIL;

	// Explosion
	CEffect_Texture::EFFECTEXTURE_DESC eEffectTexture5_Desc = {};
	ZeroMemory(&eEffectTexture5_Desc, sizeof(eEffectTexture5_Desc));
	{
		eEffectTexture5_Desc.eEffectType = CEffect_Texture::EFFECT_TYPE::ET_POT_HIT;
	}

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Effect_Texture"), &eEffectTexture5_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	// 여기 약간 느낌이 결국에서는 
	CLandObject::LANDOBJ_DESC LandObjDesc{};
	{
		LandObjDesc.pNavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecCells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation")))->Get_VecCell();

		// Mini GameA
		LandObjDesc.pGameA_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecGameA_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_MiniGameA")))->Get_VecCell();
		
		// Boss Stage
		LandObjDesc.pBossStage_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecBossStage_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_Boss_Map")))->Get_VecCell();
	}


	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), LandObjDesc)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), LandObjDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
