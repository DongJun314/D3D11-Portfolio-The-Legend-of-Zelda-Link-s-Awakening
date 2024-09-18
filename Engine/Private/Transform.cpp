#include "Transform.h"

// Engine
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{

}

void CTransform::Set_Scaling(_float _fX, _float _fY, _float _fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * _fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * _fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * _fZ);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return S_OK;

	TRANSFORM_DESC* pTransformDesc = (TRANSFORM_DESC*)_pArg;
	{
		m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
		m_fRotationPerSec = pTransformDesc->fRotationPerSec;
	}

	return S_OK;
}

void CTransform::Go_Straight(_float _fTimeDelta, CNavigation* _pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

	if( nullptr == _pNavigation || true == _pNavigation->Is_Move(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float _fTimeDelta, CNavigation* _pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

	if (nullptr == _pNavigation || true == _pNavigation->Is_Move(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float _fTimeDelta, CNavigation* _pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;

	if (nullptr == _pNavigation || true == _pNavigation->Is_Move(vPosition))
		Set_State(STATE_POSITION, vPosition);	
}

void CTransform::Go_Right(_float _fTimeDelta, CNavigation* _pNavigation)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;

	if (nullptr == _pNavigation || true == _pNavigation->Is_Move(vPosition))
		Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Up(_float _fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * _fTimeDelta;

	Set_State(STATE_POSITION, vPosition);

}

void CTransform::Go_Down(_float _fTimeDelta)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vDown = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vDown) * m_fSpeedPerSec * _fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

_bool CTransform::Go_PosLerp(_float _fTimeDelta, _vector _vTargetPos, _float _fVecOffset, _vector _vDistance)
{
	_vector vPos = Get_State(CTransform::STATE_POSITION);
	_vector vOffsetPos = _vTargetPos + _vDistance;
	_vector vDir = vOffsetPos - vPos;

	_vector m_TargetPos = _vTargetPos;

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vOffsetPos));
	_float  fVec = (1 - _fTimeDelta) * 0 + _fTimeDelta * fLength;

	vPos += XMVector3Normalize(vDir) * fVec * m_TransformDesc.fSpeedPerSec * _fVecOffset;
	Set_State(CTransform::STATE_POSITION, vPos);

	if (fVec == 0.0f || fLength < 0.1f)
		return true;

	return false;
}

void CTransform::Turn(_fvector _vAxis, _float _fTimeDelta)
{
	// Turn 함수는 동적회전
	_matrix RotationMatrix = XMMatrixRotationAxis(_vAxis, m_fRotationPerSec * _fTimeDelta);

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_vector vStateDir = Get_State(STATE(i));

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::TurnTo(ROTATION _eRotation, _float _fTimeDelta, CNavigation* _pNavigation)
{
	_vector vUp = { 0.f, 0.f, 0.f, 0.f };
	_vector vLook = { 0.f, 0.f, 0.f, 0.f };
	_vector vRight = { 0.f, 0.f, 0.f, 0.f };

	switch (_eRotation)
	{
	case TO_FORWARD:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { 0.f, 0.f, 1.f, 0.f };
		vRight = { 1.f, 0.f, 0.f, 0.f };
		break;

	case TO_BACKWARD:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { 0.f, 0.f, -1.f, 0.f };
		vRight = {-1.f, 0.f, 0.f, 0.f };
		break;

	case TO_LEFT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { -1.f, 0.f, 0.f, 0.f };
		vRight = { 0.f, 0.f, 1.f, 0.f };
		break;

	case TO_RIGHT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { 1.f, 0.f, 0.f, 0.f };
		vRight = { 0.f, 0.f, -1.f, 0.f };
		break;

	case TO_FRIGHT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { 0.7071f, 0.f, 0.7071f, 0.f };
		vRight = { 0.7071f, 0.f, -0.7071f, 0.f };
		break;

	case TO_FLEFT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { -0.7071f, 0.f, 0.7071f, 0.f };
		vRight = { 0.7071f, 0.f, 0.7071f, 0.f };
		break;

	case TO_BRIGHT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { 0.7071f, 0.f, -0.7071f, 0.f };
		vRight = { -0.7071f, 0.f, -0.7071f, 0.f };
		break;

	case TO_BLEFT:
		vUp = { 0.f, 1.f, 0.f, 0.f };
		vLook = { -0.7071f, 0.f, -0.7071f, 0.f };
		vRight = { -0.7071f, 0.f, 0.7071f, 0.f };
		break;
	}

	Set_State(STATE_RIGHT, vRight * Get_Scaled().x);
	Set_State(STATE_UP, vUp * Get_Scaled().x);
	Set_State(STATE_LOOK, vLook * Get_Scaled().x);

	_vector vPosition = Get_State(STATE_POSITION);
	{
		vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

		if (nullptr == _pNavigation || true == _pNavigation->Is_Move(vPosition))
			Set_State(STATE_POSITION, vPosition);
	}


}

void CTransform::Rotation(_fvector _vAxis, _float _fRadian)
{
	// Rotation 함수는 정적회전
	_matrix RotationMatrix = XMMatrixRotationAxis(_vAxis, _fRadian);
	_float3 vScaled = Get_Scaled();

	for (size_t i = STATE_RIGHT; i < STATE_POSITION; i++)
	{
		_float4 vTmp = _float4(0.f, 0.f, 0.f, 0.f);

		*((_float*)&vTmp + i) = 1.f * (*((_float*)&vScaled + i));

		_vector vStateDir = XMLoadFloat4(&vTmp);

		Set_State(STATE(i), XMVector4Transform(vStateDir, RotationMatrix));
	}
}

void CTransform::MoveTo(_fvector _vPoint, _float _fLimit, _float _fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDir = _vPoint - vPosition;

	_float fDistance = XMVector3Length(vDir).m128_f32[0];

	if (fDistance >= _fLimit) // 이건 일정거리까지만 따라다니는
	{
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * _fTimeDelta;
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::LookAt(_fvector _vPoint)
{
	// 순수하게 몸을 기울여서 쳐다보기 위한 함수, 룩을 구한뒤, 외적으로 통해 라업룩을 구한뒤, 행렬에 집어넣기
	_float3 vScaled = Get_Scaled();
	
	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	
	_vector vLook = _vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::LookAt_ForLandObject(_fvector _vPoint)
{
	// 몬스터 혹은 플레이어가 위에 있는 쪽으로 몸을 회전시키기 위한 함수 
	_float3	vScaled = Get_Scaled();

	_vector	vPosition = Get_State(CTransform::STATE_POSITION);

	_vector	vLook = _vPoint - vPosition;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

HRESULT CTransform::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName)
{
	return _pShader->Bind_Matrix(_pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CTransform* pInstance = new CTransform(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* _pArg)
{
	return nullptr;
}

void CTransform::Free()
{
	__super::Free();
}
