#pragma once

#include "Base.h"
#include "Editor_Defines.h"

#include "ImGuizmo.h"

BEGIN(Editor)

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

public:
	enum MODE { LOCAL, WORLD };
	enum OPERATION { TRANSLATE, ROTATE, SCALE };

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, class CGameInstance* _pGameInstance);
	void	BeginTick();
	void	Tick(_float _fTimeDelta);
	void	EndTick();
	void	Render();
	


#pragma region Getter Setter
public:
	void Set_Camera(class CCamera_Free* _pCamera) { m_pCamera = _pCamera; }
	CCamera_Free* Get_Camera() { return m_pCamera; }

	void Set_MeshTerrain(class CMesh_Terrain* _pTerrain) { m_pMeshTerrain = _pTerrain; }
	CMesh_Terrain* Get_MeshTerrain() { return m_pMeshTerrain; }

	const _float4& Get_CanvasColor() { return m_fCanvasColor; }

	void Set_strFilePath(string _strFilePath) { m_strFilePath = _strFilePath; }
	const string Get_strFilePath() { return m_strFilePath; }

	void Set_IsClicked(_bool _bIsClicked) { m_bIsClicked = _bIsClicked; }
	const _bool Get_IsClicked() { return m_bIsClicked; }

	void Set_CurOBJ(class CGameObject* _pOBJ) { m_pOBJ = _pOBJ; }
	class CGameObject* Get_CurOBJ() { return m_pOBJ; }

	void Set_OBJ(wstring _wstrKey, class CGameObject* _pOBJ, wstring* _wstrKeyValue);
	class CGameObject* Get_OBJ(wstring _wstrKey);

	void Set_szFileName(char* _szFileName) { strcpy_s(m_szFileName, _szFileName); }
	const char* Get_szFileName() { return m_szFileName; }

#pragma endregion

public:
	void Load_File();
	void Save_File(const char* _szFileName);



public:
	virtual void Free() override;

	///////////////////////////////////////////////////
	////////////////* Member Variable *////////////////
	///////////////////////////////////////////////////


private:
	bool	m_bMode = true;
	_float4 m_fCanvasColor = { 0.8f,0.89f,1.f,1.f };


	// LevelGamePlayer
	_bool m_bIsClicked = false;
	string	m_strFilePath;

	// Right_Window InputText에 적은 이름
	char m_szFileName[64] = { 0 };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };
	class CCamera_Free* m_pCamera = { nullptr };
	class CMesh_Terrain* m_pMeshTerrain = { nullptr };



	class CGameObject* m_pOBJ = { nullptr };
	map<wstring, class CGameObject*> m_mapOBJ;
};

END

