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

	// ���߿� ū�Ÿ� ã�Ƽ� ��´�, �� �ִ� Ű�������� ��� ����
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

	// �� ���ǹ��� ���� �ǹ̳ĸ�, ���� �ִϸ��̼��� ������, ������ ����� ���� ���·� ���ʰ� ���ӵ� ������ ���̳���, �׷��⿡ LastKeyFrame.Time���� ũ�ų� ���� ��� ���ǹ��� ����ϰԲ� �ߴ�
	if (_dTrackPosition >= LastKeyFrame.Time)
	{
		*_pCurrentKeyFrame = 0;

		// ������ �����ӿ��� ���ʰ� �Ȱ��� ����� ���ϱ� ����
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);

		XMStoreFloat3(&m_tKeyFrame.vScale, vScale);
		XMStoreFloat4(&m_tKeyFrame.vRotation, vRotation);
		XMStoreFloat3(&m_tKeyFrame.vPosition, vTranslation);
	}
	else
	{
		// iCurrentKeyFrame�� ���� � �ΰ��� Ű������ ���̿� ������� ���� Ű�������� �ε����� �����ѰŴ�
		while (_dTrackPosition >= m_vecKeyFrames[*_pCurrentKeyFrame + 1].Time)
			++*_pCurrentKeyFrame;

		// ���� �ؿ��� ���� �� Ű������ ���̸� �������� �ϴ� ����̸�, ratio�� �� Ű�������� ������ ������, ���°��� ������ ���缭 �����ִ� ��Ȱ
		_double Ratio = (_dTrackPosition - m_vecKeyFrames[*_pCurrentKeyFrame].Time) / (m_vecKeyFrames[*_pCurrentKeyFrame + 1].Time - m_vecKeyFrames[*_pCurrentKeyFrame].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame].vScale), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vScale), (_float)Ratio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrame].vRotation), XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vRotation), (_float)Ratio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame].vPosition), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrame + 1].vPosition), (_float)Ratio), 1.f);
		
		XMStoreFloat3(&m_tKeyFrame.vScale, vScale);
		XMStoreFloat4(&m_tKeyFrame.vRotation, vRotation);
		XMStoreFloat3(&m_tKeyFrame.vPosition, vTranslation);
	}

	// ũ���̷� ���� �� �ִ� ����� ������ִ� �Լ�(XMMatrixAffineTransformation)
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
