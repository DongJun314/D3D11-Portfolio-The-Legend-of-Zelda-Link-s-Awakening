#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	typedef struct KeyFrame
	{
		_float3 vScale, vPosition;
		_float4 vRotation;
		_double Time;
	}KEYFRAME;

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(std::ifstream* _pReadBinary, const _char* _szModelName, const _uint _iNumKeyFrames, class CModel* _pModel);

public:
	void Invalidate_TransformationMatrix(_double _dTrackPosition, _uint* _pCurrentKeyFrame, const vector<class CBone*>& _vecBones);
	void Invalidate_Linear_TransformationMatrix(_double _dTrackPosition, _uint* _pCurrentKeyFrame, const vector<class CBone*>& _vecBones, vector<class CChannel*> _vecChannels);
	
public:
	static CChannel* Create(std::ifstream* _pReadBinary, const _char* _szModelName, const _uint _iNumKeyFrames, class CModel* _pModel);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_char m_szName[MAX_PATH] = "";
	_uint m_iNumKeyFrames = { 0 };

	_uint m_iBoneIndex = { 0 };

	vector<KEYFRAME> m_vecKeyFrames;

	// Test
	KEYFRAME m_tKeyFrame;
	_double m_dTrackPosBetweenTwoAnim = { 0 };
	_double m_dRatio = { 0 };

};

END