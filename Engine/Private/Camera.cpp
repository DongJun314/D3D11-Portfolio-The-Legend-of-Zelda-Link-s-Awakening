#include "Camera.h"

// Engine
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CCamera::CCamera(const CCamera& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* _pArg)
{
	CAMERA_DESC* pCameraDesc = (CAMERA_DESC*)_pArg;
	{
		m_fFovY = pCameraDesc->fFovY;
		m_fNear = pCameraDesc->fNear;
		m_fFar = pCameraDesc->fFar;
		m_fAspect = pCameraDesc->fAspect;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&pCameraDesc->vAt));

	return S_OK;
}

void CCamera::Priority_Tick(_float _fTimeDelta)
{
}

void CCamera::Tick(_float _fTimeDelta)
{
}

void CCamera::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::SetUp_TransformMatices()
{
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovY, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();
}
