#include "PipeLine.h"

CPipeLine::CPipeLine()
{

}

void CPipeLine::Tick()
{
	// 뷰 투영의 역형렬을 구한다
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine;
}

void CPipeLine::Free()
{
}
