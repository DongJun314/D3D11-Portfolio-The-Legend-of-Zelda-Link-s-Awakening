#include "..\Public\GameInstance.h"

// Device
#include "Input_Device.h"
#include "Graphic_Device.h"

// Manager
#include "Font_Manager.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Light_Manager.h"
#include "Sound_Manager.h"
#include "Object_Manager.h"
#include "KeyInput_Manager.h"
#include "Resource_Manager.h"
#include "Component_Manager.h"

// Engine
#include "Picking.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE _hInst, _uint _iNumLevels, const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(_GraphicDesc, _ppDevice, _ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*_ppDevice, *_ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(_hInst, _GraphicDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create(*_ppDevice, *_ppContext);
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*_ppDevice, *_ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;


	m_pResource_Manager = CResource_Manager::Create();
	if (nullptr == m_pResource_Manager)
		return E_FAIL;

	m_pKeyInput_Manager = CKeyInput_Manager::Create();
	if (nullptr == m_pKeyInput_Manager)
		return E_FAIL;



	m_pObject_Manager = CObject_Manager::Create(_iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(_iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;


	/// Gara Picking	함수
	m_pPicking = CPicking::Create(*_ppDevice, *_ppContext, _GraphicDesc);
	if (nullptr == m_pPicking)
		return E_FAIL;



	return S_OK;
}

void CGameInstance::Tick_Engine(_float _fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;

	/* 다이렉트 인풋을 통해 키보드와 마우스의 상태를 저장한다 .*/
	m_pInput_Device->Tick();

	m_pObject_Manager->Priority_Tick(_fTimeDelta);

	m_pPipeLine->Tick();

	m_pObject_Manager->Tick(_fTimeDelta);

	m_pObject_Manager->Late_Tick(_fTimeDelta);

	m_pLevel_Manager->Tick(_fTimeDelta);
}

HRESULT CGameInstance::Draw1(const _float4& _vClearColor)
{
	// 오리지널
	//{
	//	if (nullptr == m_pGraphic_Device)
	//		return E_FAIL;
	//
	//	m_pGraphic_Device->Clear_BackBuffer_View(_vClearColor);
	//	m_pGraphic_Device->Clear_DepthStencil_View();
	//
	//	// 순서? 중요? 
	//	m_pRenderer->Render();
	//	m_pLevel_Manager->Render();
	//
	//	return m_pGraphic_Device->Present();
	//}

	Clear_Views(_vClearColor);
	
	Render_Managers();

	return Present_SwapChain();
}

HRESULT CGameInstance::Begin_Draw(const _float4& _vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(_vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::Draw()
{
	/* 그리기용 객체를 통해 무언가를 그린다. */
	// 순서? 중요? 
	if (FAILED(m_pRenderer->Render()))
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

void CGameInstance::Clear(_uint _iLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return;

	m_pObject_Manager->Clear(_iLevelIndex);
	m_pComponent_Manager->Clear(_iLevelIndex);
}


///////////////
/* For ImGui */
///////////////
HRESULT CGameInstance::Clear_Views(const _float4& _vClearColor)
{
	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(_vClearColor)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	Render_Managers();

	return S_OK;
}

HRESULT CGameInstance::Present_SwapChain()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Render_Managers()
{
	// 순서? 중요? 
	if (FAILED(m_pRenderer->Render()))
		return E_FAIL;

	if (FAILED(m_pLevel_Manager->Render()))
		return E_FAIL;

	return S_OK;
}


//////////////////////
/* For Input_Device */
//////////////////////
_byte CGameInstance::Get_DIKeyState(_ubyte _byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(_byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE _eMouse)
{
	return m_pInput_Device->Get_DIMouseState(_eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE _eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(_eMouseState);
}



//////////////////////
/* For Timer_Manger */
//////////////////////
_float CGameInstance::Compute_TimeDelta(const wstring& _pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Compute_TimeDelta(_pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const wstring& _pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(_pTimerTag);
}


//////////////////
/* For Renderer */
//////////////////
HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP _eRenderGroup, CGameObject* _pGameObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(_eRenderGroup, _pGameObject);
}


///////////////////////
/* For Level_Manager */
///////////////////////
HRESULT CGameInstance::Open_Level(_uint _iLevelIndex, CLevel* _pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(_iLevelIndex, _pNewLevel);
}


////////////////////////
/* For Object_Manager */
////////////////////////
HRESULT CGameInstance::Add_Prototype(const wstring& _strPrototypeTag, CGameObject* _pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(_strPrototypeTag, _pPrototype);
}

HRESULT CGameInstance::Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(_iLevelIndex, _strLayerTag, _strPrototypeTag, _pArg);
}

CGameObject* CGameInstance::Clone_GameObject(const wstring& _strPrototypeTag, void* _pArg, _uint _iLevel)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(_strPrototypeTag, _pArg, _iLevel);
}

CGameObject* CGameInstance::Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	return m_pObject_Manager->Find_CloneObject(_iLevelIndex, _strLayerTag);
}

CComponent* CGameInstance::Get_Component(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex)
{
	return m_pObject_Manager->Get_Component(_iLevelIndex, _strLayerTag, _strComTag, _iIndex);
}

CGameObject* CGameInstance::Get_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, _uint _iIndex)
{
	return m_pObject_Manager->Get_GameObject(_iLevelIndex, _strLayerTag, _iIndex);
}

void CGameInstance::Clear_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	m_pObject_Manager->Clear_CloneObject(_iLevelIndex, _strLayerTag);
}

CLayer* CGameInstance::Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
	return m_pObject_Manager->Find_Layer(_iLevelIndex, _strLayerTag);
}


///////////////////////////
/* For Component_Manager */
///////////////////////////
HRESULT CGameInstance::Add_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag, CComponent* _pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(_iLevelIndex, _strPrototypeTag, _pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, void* _pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(_iLevelIndex, _strPrototypeTag, _pArg);
}

CComponent* CGameInstance::Find_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag)
{
	return m_pComponent_Manager->Find_Prototype(_iLevelIndex, _strPrototypeTag);
}


///////////////////////
/* For Light_Manager */
///////////////////////
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& _LightDesc)
{
	return m_pLight_Manager->Add_Light(_LightDesc);
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint _iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(_iIndex);
}


//////////////////
/* For PipeLine */
//////////////////
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix)
{
	m_pPipeLine->Set_Transform(_eState, _TransformMatrix);
}

_float4 CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Float4x4(_eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Float4x4_Inverse(_eState);
}


_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Matrix(_eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(_eState);
}






////////////For Picking ////////////
_bool CGameInstance::Mouse_Picking(const _matrix& _Proj, const _matrix& _ViewInverse, const _matrix& _WorldMatrix, 
	_float3* _vPickLocalPos, _float3* _vPickWorldPos, _float* _Dis, const class CModel* _pModel, class CGameObject** _pGameObj, _vector* _vDir, _vector* _vOrigin)
{
	return m_pPicking->Mouse_Picking(_Proj, _ViewInverse, _WorldMatrix, _vPickLocalPos, _vPickWorldPos, _Dis, _pModel, _pGameObj, _vDir, _vOrigin);
}




////////////For Resource_Manager ////////////
HRESULT CGameInstance::For_Converter()
{
	m_pResource_Manager = CResource_Manager::Create();
	if (nullptr == m_pResource_Manager)
		return E_FAIL;

	return S_OK;
}

vector<string> CGameInstance::Get_VecStrFilePaths(CResource_Manager::TYPE _eType) const
{
	return m_pResource_Manager->Get_VecStrFilePaths(_eType);
}

vector<wstring> CGameInstance::Get_VecWstrFilePaths(CResource_Manager::TYPE _eType) const
{
	return m_pResource_Manager->Get_VecWstrFilePaths(_eType);
}


////////////For KeyInput_Manager ////////////
_bool CGameInstance::Key_Up(_int _iKey)
{
	return m_pKeyInput_Manager->Key_Up(_iKey);
}

_bool CGameInstance::Key_Down(_int _iKey)
{
	return m_pKeyInput_Manager->Key_Down(_iKey);
}

_bool CGameInstance::Key_Pressing(_int _iKey)
{
	return m_pKeyInput_Manager->Key_Pressing(_iKey);
}

HRESULT CGameInstance::Add_Font(const wstring& _strFontTag, const wstring& _strFontFilePath)
{
	return m_pFont_Manager->Add_Font(_strFontTag, _strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& _strFontTag, const wstring& _strText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
	return m_pFont_Manager->Render(_strFontTag, _strText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

void CGameInstance::PlaySoundEx(string _strSoundKey, SOUNDTYPE _eSoundType, _float _fVolume)
{
	m_pSound_Manager->PlaySoundEx(_strSoundKey, _eSoundType, _fVolume);
}

void CGameInstance::PlayBGMLoop(string _strSoundKey, _float _fVolume)
{
	m_pSound_Manager->PlayBGMLoop(_strSoundKey, _fVolume);
}

void CGameInstance::StopSound(SOUNDTYPE _eSoundType)
{
	m_pSound_Manager->StopSound(_eSoundType);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::SetChannelVolume(SOUNDTYPE _eSoundType, _float _fVolume)
{
	m_pSound_Manager->SetChannelVolume(_eSoundType, _fVolume);
}



//////////
/* Else */
//////////
void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Release_Manager();

	DestroyInstance();
}

void CGameInstance::Release_Manager()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pKeyInput_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pResource_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
}

void CGameInstance::Free()
{
}