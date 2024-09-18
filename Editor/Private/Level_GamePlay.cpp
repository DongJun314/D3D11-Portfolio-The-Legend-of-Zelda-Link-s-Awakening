#include "pch.h"
#include "Level_GamePlay.h"

// Editor
#include "Camera_Free.h"
#include "Mesh_Terrain.h"
#include "ImGui_Manager.h"

// Engine
#include "Transform.h"
#include "GameObject.h"

// For Reading Binary Files
#include <fstream>

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLevel(_pDevice, _pContext)
	, m_pImGui_Manager(CImGui_Manager::GetInstance())
{
	Safe_AddRef(m_pImGui_Manager);
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Navi"), TEXT("Prototype_GameObject_NaviOBJ"))))
		return E_FAIL;

	//if (FAILED(Ready_Map_OBJ()))
	//	return E_FAIL;

	/*if (FAILED(Ready_Layer_Mesh_Terrain(TEXT("Layer_Mesh_Terrain"))))
		return E_FAIL;*/

	//if (FAILED(Ready_Layer_Terrain(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	
	//if (FAILED(Ready_Layer_ForkLift(TEXT("Layer_ForkLift"))))
	//	return E_FAIL;

	//CGameObject* pGameInstance = m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Com_Transform"));

	//pGameInstance->Find_Component(TEXT("Com_Transform"));
	

	return S_OK;
}

void CLevel_GamePlay::Tick(_float _fTimeDelta)
{

#pragma region 

	// Load 버튼 클릭 여부
	if (true == m_pImGui_Manager->Get_IsClicked())
	{
		// 이줄은 내가 Load Text창의 어떤 이름을 입력했다는 의미
		if (strcmp(CImGui_Manager::GetInstance()->Get_szFileName(), "") != 0)
		{
			string strFileName = CImGui_Manager::GetInstance()->Get_szFileName();
			
			wstring wstrFileName;
			wstrFileName.assign(strFileName.begin(), strFileName.end());

			Ready_Map_OBJ(wstrFileName);
		}
		else // 이줄은 내가 Load창에 입력을 한게 아닌, 그저 list에 있는 Mesh 파일을 선택했다는 의미
		{
			// string으로 되어 있는 file path를 file Name을 얻어온다.
			char szFileName[MAX_PATH];
			string strFilePath = m_pImGui_Manager->Get_strFilePath();
			_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
			string strFileName = szFileName;

			// string을 wstring으로 바꿉니다
			wstring wstrFileName, wstrFilePath;
			wstrFileName.assign(strFileName.begin(), strFileName.end());
			wstrFilePath.assign(strFilePath.begin(), strFilePath.end());

			// 원형 컴포넌트를 컴포넌트 매니저의 맵에 추가합니다
			m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Object" + wstrFileName),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, wstrFilePath));

			// 원형객체의 사본을 만들고, Layer에 저장해 관리합니다
			m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_GameObject_Object"));
		
			//CGameObject* pGameObject
		}

		m_pImGui_Manager->Set_IsClicked(false);
	}

#pragma endregion
	
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
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));
	{
		CameraDesc.fMouseSensor = 0.05f;
		CameraDesc.vEye = _float4(0.0f, 30.f, -25.f, 1.f);
		CameraDesc.vAt = _float4(0.0f, 0.0f, 0.f, 1.f);
		CameraDesc.fFovY = XMConvertToRadians(60.0f);
		CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
		CameraDesc.fNear = 0.2f;
		CameraDesc.fFar = 1000.f;
		CameraDesc.fSpeedPerSec = 10.f;
		CameraDesc.fRotationPerSec = XMConvertToRadians(70.0f);
		//CameraDesc.WorldMatrix = dynamic_cast<CTransform*>(m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Com_Transform"))->Find_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
	}

	if(FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	//m_pImGui_Manager->Set_Camera(dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"))));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map_OBJ(wstring _wstrFileName)
{
	std::ifstream ReadBinary(TEXT("../../Client/Bin/Resources/Binary/NonAnim/") + _wstrFileName, ios::in | ios::binary);

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
		strFileName = strFileName.substr(0, first);

		// string을 wstring으로 바꿉니다
		wstring wstrFileName;
		wstrFileName.assign(strFileName.begin(), strFileName.end());



		// 원형 컴포넌트를 컴포넌트 매니저의 맵에 추가합니다 
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Object_" + wstrUniqueKey),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Client/Bin/Resources/Binary/NonAnim/" + wstrFileName + TEXT(".bin"))))))
			return E_FAIL;

		// 원형객체의 사본을 만들고, Layer에 저장해 관리합니다
		if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Obj"), TEXT("Prototype_GameObject_MapOBJ"), &wstrUniqueKey)))
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

		wstring wstrKeyValue;
		CImGui_Manager::GetInstance()->Set_OBJ(wstrFileName, pGameObject, &wstrKeyValue);
		pGameObject->Set_KeyValue(wstrKeyValue);
	}

	ReadBinary.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_ForkLift(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Terrain(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Mesh_Terrain(const wstring& _strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, _strLayerTag, TEXT("Prototype_GameObject_Mesh_Terrain"))))
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

	Safe_Release(m_pImGui_Manager);

}
