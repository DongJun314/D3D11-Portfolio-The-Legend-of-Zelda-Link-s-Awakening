#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBounding(_pDevice, _pContext)
{
}

HRESULT CBounding_Sphere::Initialize(void* _pArg)
{
	/* 초기상태는 로컬상태 */
	SPHERE_DESC* pSphereDesc = (SPHERE_DESC*)_pArg;
	{
		m_pOriginalSphere = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
		m_pSphere = new BoundingSphere(*m_pOriginalSphere);
	}

	return S_OK;
}

void CBounding_Sphere::Tick(_fmatrix _WorldMatrix)
{
	m_pOriginalSphere->Transform(*m_pSphere, _WorldMatrix);
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* _pBatch)
{
	DX::Draw(_pBatch, *m_pSphere, m_bIsColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::TYPE _eType, CBounding* _pBounding)
{
	m_bIsColl = false;

	void* pTargetBounding = _pBounding->Get_Bounding();

	switch (_eType)
	{
	case CCollider::TYPE_AABB:
		m_bIsColl = m_pSphere->Intersects(*(BoundingBox*)pTargetBounding);
		break;
	case CCollider::TYPE_OBB:
		m_bIsColl = m_pSphere->Intersects(*(BoundingOrientedBox*)pTargetBounding);
		break;
	case CCollider::TYPE_SPHERE:
		m_bIsColl = m_pSphere->Intersects(*(BoundingSphere*)pTargetBounding);
		break;
	}

	return m_bIsColl;
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere);
	Safe_Delete(m_pOriginalSphere);
}
