#include "Resource_Manager.h"

CResource_Manager::CResource_Manager()
{
}

HRESULT CResource_Manager::Initialize()
{
    // 텍스처 경로를 위해 메모리를 예약합니다.
    m_vecStrFilePaths[TYPE_MESH].reserve(1024);
    m_vecWstrFilePaths[TYPE_MESH].reserve(1024);    

    // 사운드 경로를 위해 메모리를 예약합니다.
    m_vecStrFilePaths[TYPE_TEXTURE].reserve(1024);
    m_vecWstrFilePaths[TYPE_TEXTURE].reserve(1024);

    // Anim 경로를 위해 메모리를 예약합니다.
    m_vecStrFilePaths[TYPE_ANIM].reserve(1024);
    m_vecWstrFilePaths[TYPE_ANIM].reserve(1024);

    // Non_Anim 경로를 위해 메모리를 예약합니다.
    m_vecStrFilePaths[TYPE_NONANIM].reserve(1024);
    m_vecWstrFilePaths[TYPE_NONANIM].reserve(1024);


    // 리소스 경로를 읽습니다.
    Read_FilePaths(TYPE_MESH, m_fsMeshPath);
    Read_FilePaths(TYPE_TEXTURE, m_fsTexturePath);
    Read_FilePaths(TYPE_ANIM, m_fsAnimPath);
    Read_FilePaths(TYPE_NONANIM, m_fsNonAnimPath);



    // 텍스처 포인터를 만듭니다.
    //Create_TextureFromFile();
  

    return S_OK;
}

void CResource_Manager::Read_FilePaths(TYPE _eType, const std::filesystem::path& _fsFilePath)
{
    for (const filesystem::directory_entry& fsEntry : filesystem::recursive_directory_iterator(_fsFilePath))
    {
        if (filesystem::is_regular_file(fsEntry))
        {
            m_vecStrFilePaths[_eType].push_back(fsEntry.path().generic_string());
            m_vecWstrFilePaths[_eType].push_back(fsEntry.path());
        }
    }
}


void CResource_Manager::ReadSoundFilePaths(const std::filesystem::path& _fsFilePath)
{
}

CResource_Manager* CResource_Manager::Create()
{
    CResource_Manager* pInstance = new CResource_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CResource_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CResource_Manager::Free()
{
}
