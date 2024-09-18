#include "Timer_Manager.h"

// Engine
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Compute_TimeDelta(const wstring& _strpTimerTag)
{
	CTimer* pTimer = Find_Timer(_strpTimerTag);
	if (nullptr == pTimer)
		return 0.0f;

	return pTimer->Compute_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const wstring& _strpTimerTag)
{
	CTimer* pTimer = Find_Timer(_strpTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.insert({ _strpTimerTag, pTimer });

	return S_OK;
}

CTimer* CTimer_Manager::Find_Timer(const wstring& _strpTimerTag)
{
	auto iter = m_mapTimers.find(_strpTimerTag);
		
	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager();
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_mapTimers)
		Safe_Release(Pair.second);

	m_mapTimers.clear();
}