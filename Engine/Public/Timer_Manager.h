#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	_float		Compute_TimeDelta(const wstring& _strpTimerTag);

public:
	HRESULT		Add_Timer(const wstring& _strpTimerTag);

private:
	class CTimer* Find_Timer(const wstring& _strpTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	map<const wstring, class CTimer*>		m_mapTimers;
};

END

