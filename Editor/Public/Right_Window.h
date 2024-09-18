#pragma once

#include "Window.h"
#include "Editor_Defines.h"

BEGIN(Editor)

class CRight_Window final : public CWindow
{
private:
	CRight_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CRight_Window() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	HRESULT Show_Resources_List();
	void Save_And_Load();
	string Find_FilePath(const string& _strFileName, CResource_Manager::TYPE _eType);

#pragma region TEXTURE
public:
	HRESULT Show_Image(const string& _strFileName);
	LOADTEXTURE Load_Texture(ID3D11Device* _pDevice, const string& _strFilePath);
#pragma endregion



#pragma region MESH
	HRESULT Edit_Coord(const string& _strFileName);
	void EditTransform(_float4x4 _WorldMatrix);
#pragma endregion

public:
	static CRight_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/

private:
	_bool m_bResource = true;
	_bool m_bTextureIsSelected = false;
	_bool m_bMeshIsSelected = false;

	// 스폰 데이터 파일 이름
	char m_chFileName[64] = { 0 };

	// 모든 파일의 상대 경로
	// char* 타입의 텍스처 파일 경로
	std::vector<const char*> m_vecPNGFileNames;	
	std::vector<const char*> m_vecDDSFileNames;
	std::vector<const char*> m_vecJPGFileNames;
	std::vector<const char*> m_vecBMPFileNames;

	// char* 타입의 메시 텍스처 파일 경로
	std::vector<const char*> m_vecBINFileNames;

private:
	class CImGui_Manager* m_pImGui_Manager = { nullptr };
};

END