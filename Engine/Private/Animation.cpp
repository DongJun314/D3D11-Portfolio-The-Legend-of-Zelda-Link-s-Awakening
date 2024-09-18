#include "Animation.h"

// Engine
#include "Channel.h"
#include <fstream>

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& _rhs)
	: m_iNumAnimations(_rhs.m_iNumAnimations)
	, m_dDuration(_rhs.m_dDuration)
	, m_dTickPerSecond(_rhs.m_dTickPerSecond)
	, m_dTrackPosition(_rhs.m_dTrackPosition)
	, m_iNumChannels(_rhs.m_iNumChannels)
	, m_vecCurrentKeyFrames(_rhs.m_vecCurrentKeyFrames)
	, m_bIsFinished(_rhs.m_bIsFinished)
	, m_vecChannels(_rhs.m_vecChannels)
{
	for (auto& pChannel : m_vecChannels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, _rhs.m_szName);
}

HRESULT CAnimation::Initialize(std::ifstream* _pReadBinary, const _char* _szName, const _float _fDuration, const _float _fTickPerSecond, const _uint _iNumChannels, class CModel* _pModel)
{
	strcpy_s(m_szName, _szName);

	// 애니메이션 전체를 수행하는 길이, 쉽게 말하면 애니메이션 전체를 구동하는데 걸리는 시간(단 시간은 아니다), 이걸 통해서 한 애니메이션 동작이 끝났는지 안끝났는지 알 수 있다
	m_dDuration = _fDuration;

	// 이 애니메이션의 재생 속도, 초당 얼마나 재생하는지, 특정 애니메이션 재생 속도를 빠르게 혹은 느리게 하고 싶을 경우 다른 방법도 있지만, mTicksPerSecond를 제어해서 할 수도 있다
	m_dTickPerSecond = _fTickPerSecond;

	/* 이 애니메이션을 구동하는데 필요한 뼈의 갯수. */
	m_iNumChannels = _iNumChannels;
	
	m_vecCurrentKeyFrames.resize(m_iNumChannels);

	for (size_t j = 0; j < m_iNumChannels; j++)
	{
		_char szModelName[MAX_PATH];
		_pReadBinary->read(reinterpret_cast<char*>(szModelName), sizeof(szModelName));

		_uint iNumKeyFrames;
		_pReadBinary->read(reinterpret_cast<char*>(&iNumKeyFrames), sizeof(_uint));

		/* 이 애니메이션을 구동하는데 필요한 뼈들의 정보를 생성한다.*/
		CChannel* pChannel = CChannel::Create(_pReadBinary, szModelName, iNumKeyFrames, _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannels.push_back(pChannel);
	}


	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _bool _bIsLoop)
{
	m_dTrackPosition += m_dTickPerSecond * _fTimeDelta;

	if (m_dTrackPosition >= m_dDuration)
	{
		if (false == _bIsLoop)
		{
			m_bIsFinished = true;
			return;
		}
		else
		{
			m_dTrackPosition = 0.0;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_vecChannels[i]->Invalidate_TransformationMatrix(m_dTrackPosition, &m_vecCurrentKeyFrames[i], _vecBones);
	}
}

void CAnimation::Test_Invalidate_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _bool _bIsLoop, _uint* _iCurrentAnimation, _uint* _iNextAnimation)
{
	m_dTrackPosition += m_dTickPerSecond * _fTimeDelta;

	if (m_dTrackPosition >= m_dDuration)
	{
		if (false == _bIsLoop)
		{
			m_bIsFinished = true;
			return;
		}
		else
		{
			//_iNextAnimation = 0;
			m_dTrackPosition = 0.0;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_vecChannels[i]->Invalidate_TransformationMatrix(m_dTrackPosition, &m_vecCurrentKeyFrames[i], _vecBones);
	}	
}

void CAnimation::Invalidate_Linear_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, CAnimation* _pAnimations,  _uint* _iCurrentAnimation, _uint* _iNextAnimation)
{
	//m_dTrackPosition;
	m_dTrackPosBetweenTwoAnim += m_dTickPerSecond * _fTimeDelta;

	if (m_dTrackPosBetweenTwoAnim >= 5)
	{
 		m_bIsFinished = true;
		m_dTrackPosition = 0;
		m_dTrackPosBetweenTwoAnim = 0;

		*_iCurrentAnimation = *_iNextAnimation;

		for (auto& vecCurrentKeyFrames : m_vecCurrentKeyFrames)
			vecCurrentKeyFrames = { 0 };

		return;
	}

	for (size_t i = 0; i < m_iNumChannels; i++) // 기존 저장되어 첫번째 애니메이션에 있는 뼈들의 정보를 사용하기 위함
	{
		m_vecChannels[i]->Invalidate_Linear_TransformationMatrix(m_dTrackPosBetweenTwoAnim, &m_vecCurrentKeyFrames[i], _vecBones, _pAnimations->m_vecChannels);
	}
}

CAnimation* CAnimation::Create(std::ifstream* _pReadBinary, const _char* _szName, const _float _fDuration, const _float _fTickPerSecond, const _uint _iNumChannels, class CModel* _pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(_pReadBinary, _szName, _fDuration, _fTickPerSecond, _iNumChannels, _pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannels)
		Safe_Release(pChannel);

	m_vecChannels.clear();
}
