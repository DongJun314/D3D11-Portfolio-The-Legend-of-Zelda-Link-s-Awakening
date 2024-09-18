#include "pch.h"
#include "Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& _rhs)
	: CCamera(_rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* _pArg)
{
	CAMERA_FREE_DESC* pCameraDesc = (CAMERA_FREE_DESC*)_pArg;

	m_fMouseSensor = pCameraDesc->fMouseSensor;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Tick(_float _fTimeDelta)
{
	// 키보드
	{
		if (GetKeyState('W') & 0x8000)
		{
			m_pTransformCom->Go_Straight(_fTimeDelta);
		}

		if (GetKeyState('S') & 0x8000)
		{
			m_pTransformCom->Go_Backward(_fTimeDelta);
		}

		if (GetKeyState('A') & 0x8000)
		{
			m_pTransformCom->Go_Left(_fTimeDelta);
		}

		if (GetKeyState('D') & 0x8000)
		{
			m_pTransformCom->Go_Right(_fTimeDelta);
		}

		if (GetKeyState('Q') & 0x8000)
		{
			m_pTransformCom->Go_Down(_fTimeDelta);
		}

		if (GetKeyState('E') & 0x8000)
		{
			m_pTransformCom->Go_Up(_fTimeDelta);
		}

	


		// 현재 m_bMouseMove = false; 마우스 안움직임

		if (GetKeyState('1') & 0x8000)
		{
			m_bMouseMove = !m_bMouseMove; // 토글
		}
		if (GetKeyState('2') & 0x8000)
		{
			m_bMouseMove = !m_bMouseMove; // 토글
		}
	}

	// 마우스
	{
		_long	MouseMove = { 0 };

		if (m_bMouseMove)
		{
			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);
			}

			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensor);
			}
		}

		//if (GetKeyState(VK_LBUTTON) & 0x8000)
		//{
		//	m_pGameInstance->Transform_ToWorldSpace(m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW), m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ));

		//	//m_pGameInstance->Transform_ToLocalSpace(dynamic_cast<CTransform*>(m_pGameInstance->Find_CloneObject
		// (LEVEL_GAMEPLAY, TEXT("Com_Transform"))->Find_Component(TEXT("Com_Transform")))->Get_WorldMatrix());


		//	
		//}
	}




	if (FAILED(SetUp_TransformMatices()))
		return;
}

void CCamera_Free::Tick(_float _fTimeDelta)
{
}

void CCamera_Free::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* _pArg, _uint _iLevel)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}
