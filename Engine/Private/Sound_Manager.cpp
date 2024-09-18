#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	FMOD_System_Create(&m_pFmodSystem, FMOD_VERSION);

	// 1. �ý��� ������, 2. ����� ����ä�� �� , �ʱ�ȭ ���) 
	FMOD_System_Init(m_pFmodSystem, 32, FMOD_INIT_NORMAL, NULL);

	//LoadSoundFile();
	LoadSoundFiles("../Bin/Sounds");

	return S_OK;
}

void CSound_Manager::PlaySoundEx(string _strSoundKey, SOUNDTYPE _eSoundType, _float _fVolume)
{
	WCHAR tSoundKey[128] = L"";    // ��ȯ��ų ���ڿ��� ����
	size_t sLength = 0, sConvertedChars = 0; // ���� ���ڿ� ����, ��ȯ�� ���ڿ� ����

	sLength = strlen(_strSoundKey.c_str()) + 1;  // ��ȯ��ų ���ڿ��� ���̸� ����
	mbstowcs_s(&sConvertedChars, tSoundKey, sLength, _strSoundKey.c_str(), _TRUNCATE); // ��ȯ ��, ��ȯ�� _strSoundKey�� WCHAR tSoundKey[128] = L""; ���⿡ ���� �־���

	// ������ �ش� Ű���� WCHAR ������ ��ȯ �����ִ� ����
	map<_tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapFmodSound.begin(), m_mapFmodSound.end(), [&](auto& iter)->_bool
		{
			return !lstrcmp(tSoundKey, iter.first);
		});
	if (iter == m_mapFmodSound.end()) { return; }

	FMOD_Channel_IsPlaying(m_pFmodChannel[(INT32)_eSoundType], &IsPlaying);
	if (IsPlaying != 1)
	{
		FMOD_System_PlaySound(m_pFmodSystem, iter->second, FALSE, FALSE, &m_pFmodChannel[(INT32)_eSoundType]);
		FMOD_Channel_SetMode(m_pFmodChannel[(INT32)_eSoundType], FMOD_DEFAULT);
		FMOD_Channel_SetVolume(m_pFmodChannel[(INT32)_eSoundType], _fVolume);
		FMOD_System_Update(m_pFmodSystem);
	}	
}

void CSound_Manager::PlayBGMLoop(string _strSoundKey, _float _fVolume)
{
	WCHAR tSoundKey[128] = L"";    // ��ȯ��ų ���ڿ��� ����
	size_t sLength = 0, sConvertedChars = 0; // ���� ���ڿ� ����, ��ȯ�� ���ڿ� ����

	sLength = strlen(_strSoundKey.c_str()) + 1;  // ��ȯ��ų ���ڿ��� ���̸� ����
	mbstowcs_s(&sConvertedChars, tSoundKey, sLength, _strSoundKey.c_str(), _TRUNCATE); // ��ȯ ��, ��ȯ�� _pSoundKey�� TCHAR tSoundKey[128] = L""; ���⿡ ���� �־���

	// ������ �ش� Ű���� WCHAR ������ ��ȯ �����ִ� ����
	map<_tchar*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapFmodSound.begin(), m_mapFmodSound.end(), [&](auto& iter)->_bool
		{
			return !lstrcmp(tSoundKey, iter.first);
		});
	if (iter == m_mapFmodSound.end()) { return; }

	FMOD_System_PlaySound(m_pFmodSystem, iter->second, FALSE, FALSE, &m_pFmodChannel[(INT32)SOUNDTYPE::BGM]);
	FMOD_Channel_SetMode(m_pFmodChannel[(INT32)SOUNDTYPE::BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pFmodChannel[(INT32)SOUNDTYPE::BGM], _fVolume);
	FMOD_System_Update(m_pFmodSystem);
}

void CSound_Manager::StopSound(SOUNDTYPE _eSoundType)
{
	FMOD_Channel_Stop(m_pFmodChannel[(INT32)_eSoundType]);
}

void CSound_Manager::StopAll()
{
	for (INT32 i = 0; i < (INT32)SOUNDTYPE::ENUM_END; ++i)
		FMOD_Channel_Stop(m_pFmodChannel[i]);
}

void CSound_Manager::SetChannelVolume(SOUNDTYPE _eSoundType, _float _fVolume)
{
	FMOD_Channel_SetVolume(m_pFmodChannel[(INT32)_eSoundType], _fVolume);
	FMOD_System_Update(m_pFmodSystem);
}

HRESULT CSound_Manager::LoadSoundFiles(string _strPath)
{
	_finddata_t fd;
	// �־��� ��ο��� ��ġ�ϴ� ������ �ִ��� ������ ã�� 
	intptr_t handle = _findfirst((_strPath + "/*.*").c_str(), &fd);

	// ���ǿ� ���� �ʴٸ� -1�� ����
	if (handle == -1) { return E_FAIL; }

	INT32 iResult = 0;

	while (iResult != -1)
	{
		// attrib�� ���� �Ǵ� ���丮�� �Ӽ��� ��Ÿ����, _A_SUBDIR�� ���丮�� ��Ÿ���ϴ�. ��, fd.attrib�� ���丮�̸� true
		if (fd.attrib & _A_SUBDIR)
		{
			if (strcmp(fd.name, ".") != 0 && strcmp(fd.name, "..") != 0)
			{
				// ���� ��� + ���ο� ���� �̸�
				string strNewPath = _strPath + "/" + fd.name;
				// ��������� ���� Ž��
				LoadSoundFiles(strNewPath.c_str());
			}
		}
		else
		{
			string strFullPath = _strPath + "/" + fd.name;
			FMOD_SOUND* pSound = nullptr;
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pFmodSystem, strFullPath.c_str(), FMOD_DEFAULT, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				size_t iLength = strlen(fd.name) + 1;
				WCHAR* pSoundKey = new WCHAR[(INT32)iLength];
				ZeroMemory(pSoundKey, sizeof(WCHAR) * (INT32)iLength);
				MultiByteToWideChar(CP_ACP, 0, fd.name, (INT32)iLength, pSoundKey, (INT32)iLength);
				m_mapFmodSound.emplace(pSoundKey, pSound);
			}
		}
		iResult = _findnext(handle, &fd);
	}
	FMOD_System_Update(m_pFmodSystem);

	_findclose(handle);

	return S_OK;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;

	return nullptr;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapFmodSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapFmodSound.clear();

	FMOD_System_Release(m_pFmodSystem);
	FMOD_System_Close(m_pFmodSystem);
}
