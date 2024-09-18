#pragma once

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT		Ready_Timer();
	_float		Compute_TimeDelta();

public:
	static CTimer* Create();
	virtual void	Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float					m_fTimeDelta = {};

	LARGE_INTEGER			m_FrameTime  = {};
	LARGE_INTEGER			m_FixTime	 = {};
	LARGE_INTEGER			m_LastTime   = {};
	LARGE_INTEGER			m_CpuTick    = {};
};

END