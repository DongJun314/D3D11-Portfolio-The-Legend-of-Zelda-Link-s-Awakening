#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& _rhs);
	virtual ~CAnimation() = default;

public:
	void Set_TickPerSec(_double _TickPerSec) {
		m_dTickPerSecond = _TickPerSec;
	}

	void Set_TrackPosition(_double _TrackPosition) {
		m_dTrackPosition = _TrackPosition;
	}
	_double Get_Duration() {
		return m_dDuration;
	}

	_double Get_TickPerSec() {
		return m_dTickPerSecond;
	}

	_double Get_TrackPosition() {
		return m_dTrackPosition;
	}

public:
	HRESULT Initialize(std::ifstream* _pReadBinary, const _char* _szName, const _float _fDuration, const _float _fTickPerSecond, const _uint _iNumChannels, class CModel* _pModel);

public:
	void Invalidate_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _bool _bIsLoop);
	void Invalidate_Linear_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, class CAnimation* _pAnimations, _uint* _iCurrentAnimation, _uint* _iNextAnimation);

	/// <TEXT>
	void Test_Invalidate_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _bool _bIsLoop, _uint* _iCurrentAnimation, _uint* _iNextAnimation);

	/// <TEXT>

public:
	static CAnimation* Create(std::ifstream* _pReadBinary, const _char* _szName, const _float _fDuration, const _float _fTickPerSecond, const _uint _iNumChannels, class CModel* _pModel);
	CAnimation* Clone();
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_char m_szName[MAX_PATH] = "";

	_bool m_bIsFinished = { false };

	_uint m_iNumChannels = { 0 };
	_uint m_iNumAnimations = { 0 };

	_double m_dDuration = { 0.0 };
	_double m_dTickPerSecond = { 0.0 };
	_double m_dTrackPosition = { 0.0 };

	vector<_uint> m_vecCurrentKeyFrames;
	vector<class CChannel*> m_vecChannels = { };



	// Test
	_bool m_bIsPlaying = { true };
	_bool m_bIsBetween = { true };
	_double m_dTrackPosBetweenTwoAnim = { 0 };
};

END