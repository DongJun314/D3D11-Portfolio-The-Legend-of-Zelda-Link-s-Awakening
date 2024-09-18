#pragma once

/* 엔진에서 제공하는 대부분의 기능들을 가지고 있는다. */
/* Object_Manager, Level_Manager, Component_Manager, Graphic_Device, Input_Device */
/* 위와 같은 여러 클래스들의 기능들을 클라이언트에 보여주고 실제 호출도 수행한다. */

#include "Renderer.h"
#include "PipeLine.h"
#include "KeyInput_Manager.h"
#include "Resource_Manager.h"
#include "Component_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT		Initialize_Engine(HINSTANCE _hInst, _uint _iNumLevels, const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext);
	void		Tick_Engine(_float _fTimeDelta);
	HRESULT		Draw1(const _float4& _vClearColor);
	HRESULT		Begin_Draw(const _float4& _vClearColor);
	HRESULT		Draw();
	HRESULT		End_Draw();

	void		Clear(_uint _iLevelIndex);

public: /* For ImGui */
	HRESULT		Clear_Views(const _float4& _vClearColor);
	HRESULT		Render_Managers();
	HRESULT		Present_SwapChain();

public: /* For Input_Device */
	_byte	Get_DIKeyState(_ubyte _byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE _eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE _eMouseState);

public: /* For Timer_Manger */
	_float		Compute_TimeDelta(const wstring& _pTimerTag);
	HRESULT		Add_Timer(const wstring& _pTimerTag);

public: /* For Renderer */
	HRESULT		Add_RenderGroup(CRenderer::RENDERGROUP  _eRenderGroup, class CGameObject* _pGameObject);

public: /* For Level_Manager */
	HRESULT		Open_Level(_uint _iLevelIndex, class CLevel* _pNewLevel);

public: /* For Object_Manager */
	HRESULT		Add_Prototype(const wstring& _strPrototypeTag, class CGameObject* _pPrototype);
	HRESULT		Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg = nullptr);
	class CLayer* Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag);
	class CGameObject* Clone_GameObject(const wstring& _strPrototypeTag, void* _pArg = nullptr, _uint _iLevel = 0);
	class CGameObject* Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag);
	class CComponent* Get_Component(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex = 0);
	class CGameObject* Get_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, _uint _iIndex = 0);
	void Clear_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag);

public: /* For Component_Manager */
	HRESULT				Add_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag, class CComponent* _pPrototype);
	class CComponent*	Clone_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, void* _pArg);
	class CComponent*	Find_Prototype(_uint _iLevelIndex, const wstring& _strPrototypeTag);	

public: /* For Light_Manager */
	HRESULT Add_Light(const LIGHT_DESC& _LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint _iIndex) const;

public: /* For PipeLine */
	void		Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix);
	_float4		Get_CamPosition();
	_float4x4	Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE _eState);
	_float4x4	Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState);
	_matrix		Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE _eState);
	_matrix		Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState);

public: /* For Picking */
	_bool Mouse_Picking(const _matrix& _Proj, const _matrix& _ViewInverse, const _matrix& _WorldMatrix, 
		_float3* _vPickLocalPos, _float3* _vPickWorldPos, _float* _Dis,
		const class CModel* _pModel, class CGameObject** _pGameObj, _vector* _vDir, _vector* _vOrigin);

public: /* For Resource_Manager */
	HRESULT For_Converter();
	vector<string> Get_VecStrFilePaths(CResource_Manager::TYPE _eType) const;
	vector<wstring> Get_VecWstrFilePaths(CResource_Manager::TYPE _eType) const;
	
public: /* For Key_Manager */
	_bool Key_Up(_int _iKey);
	_bool Key_Down(_int _iKey);
	_bool Key_Pressing(_int _iKey);

public: /* For Font_Manager */
	HRESULT Add_Font(const wstring& _strFontTag, const wstring& _strFontFilePath);
	HRESULT Render_Font(const wstring& _strFontTag, const wstring& _strText, const _float2& _vPosition, _fvector _vColor = Colors::White, _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), _float _fScale = 1.f);

public: /* For Sound_Manager */
	void PlaySoundEx(string _strSoundKey, SOUNDTYPE _eSoundType, _float _fVolume);
	void PlayBGMLoop(string _strSoundKey, _float _fVolume);
	void StopSound(SOUNDTYPE _eSoundType);
	void StopAll();
	void SetChannelVolume(SOUNDTYPE _eSoundType, _float _fVolume);
	 


public:
	static void		Release_Engine();
	void			Release_Manager();
	virtual void	Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	// Device
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CInput_Device*		m_pInput_Device = { nullptr };

	// Manager
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CComponent_Manager*	m_pComponent_Manager = { nullptr };
	class CLight_Manager*		m_pLight_Manager = { nullptr };
	class CResource_Manager*	m_pResource_Manager = { nullptr };
	class CKeyInput_Manager*	m_pKeyInput_Manager = { nullptr };
	class CFont_Manager*		m_pFont_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };


	class CRenderer*			m_pRenderer = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };






	///////For Picking Gara////////////////
	class CPicking* m_pPicking = { nullptr };
};

END

