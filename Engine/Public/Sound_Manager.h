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
	/* 혹시 모르니 남겨둠
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
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pFmodSystem = { nullptr };

	// 사운드 리소스 정보를 갖는 객체 
	map<_tchar*, FMOD_SOUND*> m_mapFmodSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pFmodChannel[(INT32)SOUNDTYPE::ENUM_END];
};

END