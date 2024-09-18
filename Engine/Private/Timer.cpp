#include "Timer.h"

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
}

HRESULT CTimer::Ready_Timer()
{
	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

_float CTimer::Compute_TimeDelta()
{
	QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = _float(m_FrameTime.QuadPart - m_LastTime.QuadPart) / m_CpuTick.QuadPart;

	m_LastTime = m_FrameTime;

	return m_fTimeDelta;
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX("Failed to Create : CTimer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}
