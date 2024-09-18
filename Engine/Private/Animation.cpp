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

	// �ִϸ��̼� ��ü�� �����ϴ� ����, ���� ���ϸ� �ִϸ��̼� ��ü�� �����ϴµ� �ɸ��� �ð�(�� �ð��� �ƴϴ�), �̰� ���ؼ� �� �ִϸ��̼� ������ �������� �ȳ������� �� �� �ִ�
	m_dDuration = _fDuration;

	// �� �ִϸ��̼��� ��� �ӵ�, �ʴ� �󸶳� ����ϴ���, Ư�� �ִϸ��̼� ��� �ӵ��� ������ Ȥ�� ������ �ϰ� ���� ��� �ٸ� ����� ������, mTicksPerSecond�� �����ؼ� �� ���� �ִ�
	m_dTickPerSecond = _fTickPerSecond;

	/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ���� ����. */
	m_iNumChannels = _iNumChannels;
	
	m_vecCurrentKeyFrames.resize(m_iNumChannels);

	for (size_t j = 0; j < m_iNumChannels; j++)
	{
		_char szModelName[MAX_PATH];
		_pReadBinary->read(reinterpret_cast<char*>(szModelName), sizeof(szModelName));

		_uint iNumKeyFrames;
		_pReadBinary->read(reinterpret_cast<char*>(&iNumKeyFrames), sizeof(_uint));

		/* �� �ִϸ��̼��� �����ϴµ� �ʿ��� ������ ������ �����Ѵ�.*/
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

	for (size_t i = 0; i < m_iNumChannels; i++) // ���� ����Ǿ� ù��° �ִϸ��̼ǿ� �ִ� ������ ������ ����ϱ� ����
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
