#include "stdafx.h"
#include "PartObject.h"

CPartObject::CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CPartObject::CPartObject(const CPartObject& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPartObject::Initialize(void* _pArg)
{
	PARTOBJ_DESC* pPartObjDesc = (PARTOBJ_DESC*)_pArg;
	{
		m_pParentTransform = pPartObjDesc->pParentTransfrom;
		Safe_AddRef(m_pParentTransform);
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CPartObject::Priority_Tick(_float _fTimeDelta)
{
}

void CPartObject::Tick(_float _fTimeDelta)
{
}

void CPartObject::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
	return S_OK;
}


void CPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
}
