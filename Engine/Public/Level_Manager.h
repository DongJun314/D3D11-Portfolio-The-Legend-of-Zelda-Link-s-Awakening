#pragma once

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Initialize();
	void Tick(float _fTimeDelta);
	HRESULT Render();
	HRESULT Open_Level(_uint _iLevelIndex, class CLevel* _pNewLevel);

public:
	static CLevel_Manager* Create();
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint			m_iLevelIndex = { 0 };

private:
	class CLevel*	m_pCurrentLevel = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
};

END