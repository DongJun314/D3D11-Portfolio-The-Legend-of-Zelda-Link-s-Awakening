#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	/* Ȥ�� �𸣴� ���ܵ�
	//void PlaySound(TCHAR* _pSoundKey, ESoundType _eSoundType, float _fVolume);
	//void PlayBGMLoop(TCHAR* _pSoundKey, float _fVolume);
	*/
	void PlaySoundEx(string _strSoundKey, SOUNDTYPE _eSoundType, _float _fVolume);
	void PlayBGMLoop(string _strSoundKey, _float _fVolume);
	void StopSound(SOUNDTYPE _eSoundType);
	void StopAll();
	void SetChannelVolume(SOUNDTYPE _eSoundType, _float _fVolume);

private:
	//void LoadSoundFile();
	HRESULT LoadSoundFiles(string _strPath);


public:
	static CSound_Manager* Create();
	virtual void Free() override;

private:
	FMOD_BOOL IsPlaying;
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pFmodSystem = { nullptr };

	// ���� ���ҽ� ������ ���� ��ü 
	map<_tchar*, FMOD_SOUND*> m_mapFmodSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pFmodChannel[(INT32)SOUNDTYPE::ENUM_END];
};

END