#include "Bounding_OBB.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_OBB::Initialize(void* _pArg)
{
	/* 초기상태는 로컬상태 */
	OBB_DESC* pDesc = (OBB_DESC*)_pArg;

	_float4 vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));

	m_pOriginalOBB = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pOBB = new BoundingOrientedBox(*m_pOriginalOBB);

    return S_OK;
}

void CBounding_OBB::Tick(_fmatrix _WorldMatrix)
{
	m_pOriginalOBB->Transform(*m_pOBB, _WorldMatrix);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch)
{
	DX::Draw(_pBatch, *m_pOBB, m_bIsColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding* _pBounding)
{
	m_bIsColl = false;

	void* pTargetBounding = _pBounding->Get_Bounding();

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsColl = m_pOBB->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		m_bIsColl = /*m_pOBB->Intersects(*(BoundingOrientedBox*)pTargetBounding)*/Intersect((CBounding_OBB*)_pBounding);
		break;
	case CCollider::TYPE_SPHERE:
		m_bIsColl = m_pOBB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_bIsColl;
}

CBounding_OBB::OBB_COL_DESC CBounding_OBB::Compute_OBBColDesc()
{
	_float3		vPoints[8];
	{
		m_pOBB->GetCorners(vPoints);
	}

	OBB_COL_DESC			OBBDesc{};
	{
		OBBDesc.vCenter = m_pOBB->Center;
		XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
		XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vPoints[7]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
		XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vPoints[0]) - XMLoadFloat3(&vPoints[4])) * 0.5f);
	}
	
	for (size_t i = 0; i < 3; i++)
	{
		XMStoreFloat3(&OBBDesc.vAlignAxis[i], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[i])));
	}

	return OBBDesc;
}

_bool CBounding_OBB::Intersect(CBounding_OBB* _pTargetBounding)
{
	OBB_COL_DESC		OBBDesc[2];
	{
		OBBDesc[0] = Compute_OBBColDesc();
		OBBDesc[1] = _pTargetBounding->Compute_OBBColDesc();
	}

	_float			fDistance[3];

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			fDistance[0] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			fDistance[1] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			fDistance[2] = fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]) +
				fabs(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])).m128_f32[0]);

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}

	return true;
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CBounding_OBB* pInstance = new CBounding_OBB(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance; 
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB);
	Safe_Delete(m_pOriginalOBB);
}
