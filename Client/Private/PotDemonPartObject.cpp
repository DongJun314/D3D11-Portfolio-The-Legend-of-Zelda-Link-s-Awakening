#include "stdafx.h"
#include "PotDemonPartObject.h"

CPotDemonPartObject::CPotDemonPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CPotDemonPartObject::CPotDemonPartObject(const CPotDemonPartObject& _rhs)
	: CGameObject(_rhs)
{
}

void CPotDemonPartObject::Set_Scaling(_float _fX, _float _fY, _float _fZ)
{
	_float4 vTmp, vSrc, vDst;
	XMStoreFloat4(&vTmp, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[0]) * _fX);
	memcpy(&m_WorldMatrix.m[0], &vTmp, sizeof(_float4));

	XMStoreFloat4(&vSrc, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[1]) * _fY);
	memcpy(&m_WorldMatrix.m[1], &vSrc, sizeof(_float4));

	XMStoreFloat4(&vDst, XMVector3Normalize(XMLoadFloat4x4(&m_WorldMatrix).r[2]) * _fZ);
	memcpy(&m_WorldMatrix.m[2], &vDst, sizeof(_float4));
}

HRESULT CPotDemonPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPotDemonPartObject::Initialize(void* _pArg)
{
	POTDEMON_PARTOBJ_DESC* pPartObjDesc = (POTDEMON_PARTOBJ_DESC*)_pArg;
	{
		m_pParentTransform = pPartObjDesc->pParentTransfrom;
		Safe_AddRef(m_pParentTransform);
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CPotDemonPartObject::Priority_Tick(_float _fTimeDelta)
{
}

void CPotDemonPartObject::Tick(_float _fTimeDelta)
{
}

void CPotDemonPartObject::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CPotDemonPartObject::Render()
{
	return S_OK;
}

void CPotDemonPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
}
