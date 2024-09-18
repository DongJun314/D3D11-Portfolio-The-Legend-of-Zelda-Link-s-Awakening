#include "stdafx.h"
#include "Camera_Free.h"

#include "Player.h"
#include "PartObject.h"

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

	ShowCursor(false);

	return S_OK;
}

void CCamera_Free::Priority_Tick(_float _fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;

	_bool bFirstView = pPlayer->Get_FirstView();
	_bool bMiniGameA = pPlayer->Get_MiniGameA();
	_bool bIsBossStage = pPlayer->Get_IsBossStage();

	if (m_pGameInstance->Get_DIKeyState(DIK_1) && !bFirstView)
	{
		pPlayer->Set_FirstView(true);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_3) && bFirstView)
	{
		pPlayer->Set_FirstView(false);
	}
	
	if (m_pPlayerFirstView != nullptr && bFirstView)
	{	
		// Body Player 뼈의 위치 룩 정보
		CModel* pBodyModel = dynamic_cast<CModel*>(m_pPlayerFirstView->Get_Component(TEXT("Com_Model")));// Body Player의 모델
		_float4x4* BoneMatrixPtr = pBodyModel->Get_CombinedBoneMatrixPtr("joe");
		_vector BonePos = { BoneMatrixPtr->_41, BoneMatrixPtr->_42, BoneMatrixPtr->_43 };


		// Player의 Transform 정보
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform"))); // Player의 Transform
		_matrix PlayerWorldMatrix = pPlayerTransform->Get_WorldMatrix();
		_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	
		// Body Player 뼈의 붙을 카메라의 위치를 구해줍니다
		BonePos = XMVector3TransformCoord(BonePos, PlayerWorldMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
		m_pTransformCom->Set_State(CTransform::STATE_UP, pPlayerTransform->Get_State(CTransform::STATE_UP));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, pPlayerTransform->Get_State(CTransform::STATE_LOOK));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, BonePos);

		POINT ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
		ClientToScreen(g_hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);


		// 1인칭 뼈의 라업룩포 전부다 받아옵니다, 특정 동작을 했을때만 수행합니다
		if (pPlayer->Get_JumpState()
			|| pPlayer->Get_UltJumpState()
			|| pPlayer->Get_UltSTState()
			|| pPlayer->Get_UltLPState()
			|| pPlayer->Get_UltEDState()
			|| (pPlayer->Get_CurState() == STATE_DMG_QUAKE)
			|| (pPlayer->Get_CurState() == STATE_NORMAL_SLASH_ED)
			|| (pPlayer->Get_CurState() == STATE_SPECIAL_SLASH_ED))
		{
			// Right vector
			_vector vRight = { BoneMatrixPtr->_31, BoneMatrixPtr->_32, BoneMatrixPtr->_33 };
			vRight = XMVector3TransformNormal(vRight, PlayerWorldMatrix);
			vRight = -1 * vRight; 

			// Up vector
			_vector vUp = { BoneMatrixPtr->_11, BoneMatrixPtr->_12, BoneMatrixPtr->_13 };
			vUp = XMVector3TransformNormal(vUp, PlayerWorldMatrix);

			// Look vector
			_vector vLook = { BoneMatrixPtr->_21, BoneMatrixPtr->_22, BoneMatrixPtr->_23 };
			vLook = XMVector3TransformNormal(vLook, PlayerWorldMatrix);
			vLook = -1 * vLook;
			

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
			m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
		}
		else
		{
			// 1인칭 일반 뼈 위치만 받아서 세팅해줍니다 
			_long	MouseMove = { 0 };

			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);
			}

			if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensor);
			}

			_float3	vScaled = pPlayerTransform->Get_Scaled();

			//_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			//_vector vRight = XMVector3Cross(vUp, vLook);
			//vLook = XMVector3Cross(vRight, vUp);

			_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

			pPlayerTransform->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
			pPlayerTransform->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
			pPlayerTransform->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
		}
	}
	else
	{
		//Set_FovY(60.f);

		if (!bMiniGameA && !bIsBossStage)
		{
			_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

			_matrix WorldMatrix = XMMatrixSet
			(1.f, 0.f, 0.f, 0.f,
				0.f, 0.64f, 0.77f, 0.f,
				0.f, -0.77f, 0.64f, 0.f,
				vPlayerPos.m128_f32[0], vPlayerPos.m128_f32[1] + 7.2f, vPlayerPos.m128_f32[2] - 5.6f, 1.f);

			m_pTransformCom->Set_WorldMatrix(WorldMatrix);
		}
		else if (bMiniGameA && !bIsBossStage) // 미니게임A
		{
			_float3 vScaled = m_pTransformCom->Get_Scaled();

			_vector vLook = { -0.00282698497f, -0.120624468f, 0.993954003f, 0.f };
			_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
			_vector vUp = XMVector3Cross(vLook, vRight);
			_vector vPos = { 1.08590329f, 32.8095627f, 1989.85522f, 1.f };

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight)* vScaled.x);
			m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp)* vScaled.y);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook)* vScaled.z);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		}
		else if (bIsBossStage && !bMiniGameA) // 미니게임B
		{
			Set_FovY(XMConvertToRadians(60.f));

			_float3 vScaled = m_pTransformCom->Get_Scaled();

			_vector vLook = { 0.000692371628f, -0.665255725f, 0.74831748f, 0.f };
			_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
			_vector vUp = XMVector3Cross(vLook, vRight);
			_vector vPos = { 3000.f, 55.f, -214.5f, 1.f };

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight)* vScaled.x);
			m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp)* vScaled.y);
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook)* vScaled.z);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		}
	}


	

	//if (m_pGameInstance->Get_DIKeyState(DIK_8) && m_bLock)
	//{
	//	m_bLock = false;
	//}

	//if (m_pGameInstance->Get_DIKeyState(DIK_7) && !m_bLock)
	//{
	//	m_bLock = true;
	//}

	//if (m_bLock)
	//{
	//	if (GetKeyState('W') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Straight(_fTimeDelta);
	//	}

	//	if (GetKeyState('S') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Backward(_fTimeDelta);
	//	}

	//	if (GetKeyState('A') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Left(_fTimeDelta);
	//	}

	//	if (GetKeyState('D') & 0x8000)
	//	{
	//		m_pTransformCom->Go_Right(_fTimeDelta);
	//	}

	//	_long	MouseMove = { 0 };

	//	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
	//	{
	//		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensor);
	//	}


	//	if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
	//	{
	//		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensor);
	//	}
	//}
	///*else
	//{
	//	_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

	//	_matrix WorldMatrix = XMMatrixSet
	//	(1.f, 0.f, 0.f, 0.f,
	//		0.f, 0.64f, 0.77f, 0.f,
	//		0.f, -0.77f, 0.64f, 0.f,
	//		vPlayerPos.m128_f32[0], vPlayerPos.m128_f32[1] + 7.2f, vPlayerPos.m128_f32[2] - 5.6f, 1.f);

	//	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	//}*/

	//if (m_pGameInstance->Get_DIKeyState(DIK_9) && m_bLock2)
	//{
	//	m_bLock2 = false;
	//}

	//if (m_pGameInstance->Get_DIKeyState(DIK_0) && !m_bLock2)
	//{
	//	m_bLock2 = true;
	//}

	//if (m_bLock2)
	//{
	//	_vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

	//	_matrix WorldMatrix = XMMatrixSet
	//	(1.f, 0.f, 0.f, 0.f,
	//		0.f, 0.64f, 0.77f, 0.f,
	//		0.f, -0.77f, 0.64f, 0.f,
	//		vPlayerPos.m128_f32[0], vPlayerPos.m128_f32[1] + 7.2f, vPlayerPos.m128_f32[2] - 5.6f, 1.f);

	//	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	//}

	_matrix w = m_pTransformCom->Get_WorldMatrix();	
	
	
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

void CCamera_Free::Shake_Camera(_float _fTimeDelta, _float _fShakingPower, _float _fShakingTime)
{
	// 여기서 카메라 쉐이킹 구현 
	m_fShakingTime += _fTimeDelta;

	
	if (m_iNumShakingCount < 2)
	{
		m_iNumShakingCount++;
		//m_iShakingPower = 20 * _fShakingPower + 1;
 	//	_float fRandom = XMConvertToRadians(80 + ((rand()% 1)));  // 랜덤한 X 축 쉐이킹

		//// 카메라 위치에 쉐이킹 적용
		//m_fShakingPower += fRandomX * _fShakingPower;
		//XMVectorLerp();
	/*	_double dRandom = lerp(XMConvertToRadians(80), m_iShakingPower, 0.001f);
 		Set_FovY(dRandom);*/

		CGameObject* pGameObject = m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		_vector PlayerPosition = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

		_float fRandomX = (rand() % 100) / 100.0f - 0.5f; // 랜덤한 X 축 쉐이킹
		_float fRandomY = (rand() % 100) / 100.0f - 0.5f; // 랜덤한 Y 축 쉐이킹

		// 카메라 위치에 쉐이킹 적용

		_float vX = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[0];
		_float vY = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1];
		_float vZ = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2];

		vX += fRandomX * 0.5f;
		vY += fRandomY * 0.5f; 

		_vector vPos = { vX, vY, vZ, 1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);		

	}
	else
	{
		/*m_iNumShakingCount = 0;
		m_fShakingTime = 0.f;
		m_iShakingPower = 0;

		_float3 vScaled = m_pTransformCom->Get_Scaled();

		_vector vLook = { 0.000692371628f, -0.665255725f, 0.74831748f, 0.f };
		_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_vector vUp = XMVector3Cross(vLook, vRight);
		_vector vPos = { 3000.f, 55.f, -214.5f, 1.f };

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/
	}

	//Set_FovY(XMConvertToRadians(90.f));


	// 마지막에 쉐이킹이 끝이 나면, 카메라 위치 다시 원상복구
	/*{
		CGameObject* pGameObject = m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		_vector PlayerPosition = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

		_matrix WorldMatrix = XMMatrixSet
		(1.f, 0.f, 0.f, 0.f,
			0.f, 0.64f, 0.77f, 0.f,
			0.f, -0.77f, 0.64f, 0.f,
			PlayerPosition.m128_f32[0], PlayerPosition.m128_f32[1] + 7.2f, PlayerPosition.m128_f32[2] - 5.6f, 1.f);

		m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	}*/
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

	Safe_Release(m_pPlayerFirstView);
}
