#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex, _fmatrix _PivotMatrix)
{
	// �𵨿� �߰��Ǿ� Bone�����̳ʿ� �θ���� ���°�� �߰��Ǿ� �ִ°�
	m_iParentBoneIndex = _iParentBoneIndex;

	// .data�� �����͸� ������ ���̴�
	strcpy_s(m_szName, _szName);

	// _pAIBone->mTransformation�� �θ� �������� ǥ���� ������ ���� ���, ���� �θ��� ���´� ������� �ʴ� �����̴�.
	memcpy(&m_TransformationMatrix, &_vTransformationMatrix, sizeof(_float4x4));
	memcpy(&m_CombinedTransformationMatrix, &_vCombinedTransformationMatrix, sizeof(_float4x4));

	if (-1 != _iParentBoneIndex)
	{
		// XMMatrixTranspose ��ġ���(4,1 ���Ұ� 1,4�� �ǰ� �ϴ°� ��ġ���)�� �ٲ��ش�, �޼���ǥ��� ������ ������, assimp���� ���� ������ �о� �� ���, ����� ��켱 ����� �ƴ�, ���켱 ����̱⿡, ��ġ�� ���� ��ȯ�� ����� �Ѵ�
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	}
	else
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * _PivotMatrix);


	// ���⼭ XMMatrixIdentity�� �ٸ��ɷ� �����ϴ°� �ǹ̰� ����, �ֳĸ� ���� ���°� ��� �ٲ�⿡ �ѹ� Initialize���� �������ִ� ���� ����� ũ�� ����� ����
	//XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());


	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& _vecBones)
{
	// �θ� ���� ���
	if (-1 == m_iParentBoneIndex)
		m_CombinedTransformationMatrix = m_TransformationMatrix;
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

CBone* CBone::Create(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex, _fmatrix _PivotMatrix)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(_szName, _vTransformationMatrix, _vCombinedTransformationMatrix, _iParentBoneIndex, _PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
