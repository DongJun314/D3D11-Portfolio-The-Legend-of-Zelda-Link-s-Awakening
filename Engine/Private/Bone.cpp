#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex, _fmatrix _PivotMatrix)
{
	// 모델에 추가되어 Bone컨테이너에 부모뼈가 몇번째로 추가되어 있는가
	m_iParentBoneIndex = _iParentBoneIndex;

	// .data는 포인터를 얻어오는 것이다
	strcpy_s(m_szName, _szName);

	// _pAIBone->mTransformation는 부모를 기준으로 표현된 나만의 상태 행렬, 또한 부모의 상태는 고려되지 않는 상태이다.
	memcpy(&m_TransformationMatrix, &_vTransformationMatrix, sizeof(_float4x4));
	memcpy(&m_CombinedTransformationMatrix, &_vCombinedTransformationMatrix, sizeof(_float4x4));

	if (-1 != _iParentBoneIndex)
	{
		// XMMatrixTranspose 전치행렬(4,1 원소가 1,4가 되게 하는게 전치행렬)로 바꿔준다, 왼손좌표계로 세팅은 했지만, assimp에서 집적 파일을 읽어 올 경우, 행렬이 행우선 행렬이 아닌, 열우선 행렬이기에, 전치로 먼저 취환을 해줘야 한다
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	}
	else
		XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)) * _PivotMatrix);


	// 여기서 XMMatrixIdentity를 다른걸로 세팅하는건 의미가 없다, 왜냐면 뼈는 상태가 계속 바뀌기에 한번 Initialize에서 세팅해주는 뼈는 행렬은 크게 상관이 없다
	//XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());


	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& _vecBones)
{
	// 부모가 없을 경우
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
