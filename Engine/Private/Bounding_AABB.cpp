#include "Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_AABB::Initialize(void* _pArg)
{
	/* 초기상태는 로컬상태 */
	AABB_DESC* pDesc = (AABB_DESC*)_pArg;
	{
		m_pOriginalAABB = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
		m_pAABB = new BoundingBox(*m_pOriginalAABB);
	}
		
	return S_OK;
}

void CBounding_AABB::Tick(_fmatrix _WorldMatrix)
{
	_matrix		TransformMatrix = _WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]).m128_f32[0];
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]).m128_f32[0];
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]).m128_f32[0];

	m_pOriginalAABB->Transform(*m_pAABB, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch)
{
	DX::Draw(_pBatch, *m_pAABB, m_bIsColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE _eType, CBounding* _pBounding)
{
	m_bIsColl = false;

	void* pTargetBounding = _pBounding->Get_Bounding();

	switch (_eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsColl = /*m_pAABB->Intersects(*(BoundingBox*)pTargetBounding);*/Intersect((BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		m_bIsColl = m_pAABB->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		m_bIsColl = m_pAABB->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_bIsColl;
}

_bool CBounding_AABB::Intersect(BoundingBox* _pTargetAABB)
{
	_float3		vSourMin, vSourMax;
	_float3		vDestMin, vDestMax;

	vSourMin = _float3(m_pAABB->Center.x - m_pAABB->Extents.x, m_pAABB->Center.y - m_pAABB->Extents.y, m_pAABB->Center.z - m_pAABB->Extents.z);
	vSourMax = _float3(m_pAABB->Center.x + m_pAABB->Extents.x, m_pAABB->Center.y + m_pAABB->Extents.y, m_pAABB->Center.z + m_pAABB->Extents.z);
	vDestMin = _float3(_pTargetAABB->Center.x - _pTargetAABB->Extents.x, _pTargetAABB->Center.y - _pTargetAABB->Extents.y, _pTargetAABB->Center.z - _pTargetAABB->Extents.z);
	vDestMax = _float3(_pTargetAABB->Center.x + _pTargetAABB->Extents.x, _pTargetAABB->Center.y + _pTargetAABB->Extents.y, _pTargetAABB->Center.z + _pTargetAABB->Extents.z);

	/* 너비에 대한 비교. */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;

	/* 높이에 대한 비교. */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	/* 깊이에 대한 비교. */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return false;

	return true;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CBounding_AABB* pInstance = new CBounding_AABB(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOriginalAABB);
}
