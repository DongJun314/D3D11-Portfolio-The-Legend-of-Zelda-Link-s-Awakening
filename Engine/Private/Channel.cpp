#include "Channel.h"

// Engine
#include "Bone.h"
#include "Model.h"
#include "Animation.h"

#include <fstream>

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(std::ifstream* _pReadBinary, const _char* _szModelName, const _uint _iNumKeyFrames, class CModel* _pModel)
{
 	strcpy_s(m_szName, _szModelName);

	m_iBoneIndex = _pModel->Get_BoneIndex(m_szName);

	// 둘중에 큰거를 찾아서 뱉는다, 즉 최대 키프레임을 얻기 위함
	m_iNumKeyFrames = _iNumKeyFrames;


	for (size_t k = 0; k < m_iNumKeyFrames; k++)
	{
		//
		_double KeyFrameTime;
		_pReadBinary->read(reinterpret_cast<char*>(&KeyFrameTime), sizeof(_double));

		_float3 vScale;
		_pReadBinary->read(reinterpret_cast<char*>(&vScale), sizeof(_float3));

		_float4 vRotation;
		_pReadBinary->read(reinterpret_cast<char*>(&vRotation), sizeof(_float4));

		_float3 vTranslation;
		_pReadBinary->read(reinterpret_cast<char*>(&vTranslation), sizeof(_float3));

		KEYFRAME KeyFrame;
		{
			KeyFrame.Time = KeyFrameTime;
			KeyFrame.vScale = vScale;
			KeyFrame.vRotation = vRotation;
			KeyFrame.vPosition = vTranslation;
		}

		m_vecKeyFrames.push_back(KeyFrame);
	}

	
	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(_double _dTrackPosition, _uint* _pCurrentKeyFrame, const vector<class CBone*>& _vecBones)
{
	if (0.0 == _dTrackPosition)
		*_pCurrentKeyFrame = 0;

	KEYFRAME LastKeyFrame = m_vecKeyFrames.back();

	_matrix TransformationMatrix;

	_vector vScale, vRotation, vTranslation;

	// 이 조건문은 무슨 의미냐면, 보통 애니메이션이 끝나고, 마지막 모션을 취한 상태로 몇초간 지속된 다음에 끝이난다, 그러기에 LastKeyFrame.Time보다 크거나 같을 경우 조건문을 통과하게끔 했다
	if (_dTrackPosition >= LastKeyFrame.Time)
	{
		*_pCurrentKeyFrame = 0;

		// 마지막 프레임에서 몇초가 똑같은 모션을 취하기 위함
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);

		XMStoreFloat3(&m_tKeyFrame.vScale, vScale);
		XMStoreFloat4(&m_tKeyFrame.vRotation, vRotation);
		XMStoreFloat3(&m_tKeyFrame.vPosition, vTranslation);
	}
	else
	{
		// iCurrentKeyFrame은 내가 어떤 두개의 키프래임 사이에 있을경우 왼쪽 키프래임의 인덱스를 저장한거다
		while (_dTrackPosition >= m_vecKeyFrames[*_pCurrentKeyFrame + 1].Time)
			++*_pCurrentKeyFrame;

		// 여기 밑에가 이제 그 키프레임 사이를 선형보간 하는 방법이며, ratio는 그 키프레임을 비율로 나눠서, 상태값을 비율에 맞춰서 보내주는 역활
		_double Ratio = (_dTrackPosition - m_vecKeyFrames[*_pCurrentKeyFrame].Time) / (m_vecKeyFrames[*_pCurrentKeyFrame + 1].Time - m_vecKeyFrames[*_pCurrentKeyFrame].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame].vScale), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vRotation), (_float)Ratio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vPosition), (_float)Ratio), 1.f);
		
		XMStoreFloat3(&m_tKeyFrame.vScale, vScale);
		XMStoreFloat4(&m_tKeyFrame.vRotation, vRotation);
		XMStoreFloat3(&m_tKeyFrame.vPosition, vTranslation);
	}

	// 크자이로 만들 수 있는 행렬을 만들어주는 함수(XMMatrixAffineTransformation)
	TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	_vecBones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_Linear_TransformationMatrix(_double _dTrackPosition, _uint* _pCurrentKeyFrame, const vector<class CBone*>& _vecBones, vector<class CChannel*> _vecChannels)
{
	_matrix TransformationMatrix;

	_vector vScale, vRotation, vTranslation;

	for (auto& NextAnimChannels : _vecChannels)
	{
		if (!strcmp(NextAnimChannels->m_szName, m_szName))
		{
			KEYFRAME NextAnimBeginKeyFrame = NextAnimChannels->m_vecKeyFrames.front();

			_double Ratio = _dTrackPosition / 5;

			vScale = XMVectorLerp(XMLoadFloat3(&m_tKeyFrame.vScale), XMLoadFloat3(&NextAnimBeginKeyFrame.vScale), (_float)Ratio);
			vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_tKeyFrame.vRotation), XMLoadFloat4(&NextAnimBeginKeyFrame.vRotation), (_float)Ratio);
			vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_tKeyFrame.vPosition), XMLoadFloat3(&NextAnimBeginKeyFrame.vPosition), (_float)Ratio), 1.f);

			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

			_vecBones[m_iBoneIndex]->Set_TranslationMatrix(TransformationMatrix);
		}
	}
}

CChannel* CChannel::Create(std::ifstream* _pReadBinary, const _char* _szModelName, const _uint _iNumKeyFrames, class CModel* _pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(_pReadBinary, _szModelName, _iNumKeyFrames, _pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
