#pragma once

#include "Base.h"

BEGIN(Engine)

class CResource_Manager final : public CBase
{
public:
	enum TYPE { TYPE_MESH, TYPE_TEXTURE, TYPE_MATERIAL, TYPE_ANIM, TYPE_NONANIM, TYPE_SOUND, TYPE_END };
private:
	CResource_Manager();
	virtual ~CResource_Manager() = default;

public:
	vector<string> Get_VecStrFilePaths(TYPE _eType) const {
		return m_vecStrFilePaths[_eType];
	}
	vector<wstring> Get_VecWstrFilePaths(TYPE _eType) const {
		return m_vecWstrFilePaths[_eType];
	}

public:
	HRESULT Initialize();

private:
	void Read_FilePaths(TYPE _eType, const std::filesystem::path& _fsFilePath);
	void ReadSoundFilePaths(const std::filesystem::path& _fsFilePath);

public:
	static CResource_Manager* Create();
	virtual void Free() override;


	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:

	// �޽� ���� ���
	std::filesystem::path m_fsMeshPath = L"..\\..\\Client\\Bin\\Resources\\Binary\\";

	// �ؽ��� ���� ���
	std::filesystem::path m_fsTexturePath = L"..\\..\\Client\\Bin\\Resources\\Textures\\";

	// ��Ʈ ���� ���
	std::filesystem::path m_fsFontPath = L"..\\..\\Client\\Bin\\Resources\\Fonts\\";

	// ��Ʈ ���� ���
	std::filesystem::path m_fsAnimPath = L"..\\..\\Client\\Bin\\Resources\\Binary\\Anim\\";

	// ��Ʈ ���� ���
	std::filesystem::path m_fsNonAnimPath = L"..\\..\\Client\\Bin\\Resources\\Binary\\NonAnim\\";


	// ��� ������ ��� ���
	vector<string> m_vecStrFilePaths[TYPE_END];

	// ��� ������ ��� ���
	vector<wstring> m_vecWstrFilePaths[TYPE_END];
};

END