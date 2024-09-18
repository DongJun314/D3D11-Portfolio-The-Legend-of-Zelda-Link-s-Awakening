#include "stdafx.h"
#include "Monster.h"

CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CLandObject(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _rhs)
	: CLandObject(_rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Tick(_float _fTimeDelta)
{
}

void CMonster::Tick(_float _fTimeDelta)
{
}

void CMonster::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Rotate_Monster(_bool* _bState)
{
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vToPlayerLook = XMVector3Normalize(m_vDirToPlayer);

	_vector vPlayerLocalPos = XMVector3TransformCoord(m_vPlayerPos, m_pTransformCom->Get_WorldMatrix_Inverse());
	_vector vMonsterLocalPos = XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_WorldMatrix_Inverse());

	// 플레이어가 몬스터 기준 오른쪽에 있다 -> 오른쪽으로 돌다
	if (vPlayerLocalPos.m128_f32[0] >= vMonsterLocalPos.m128_f32[0])
	{
		m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(160.f));
	}

	// 플레이어가 몬스터 기준 왼쪽에 있다 -> 왼쪽으로 돌다
	else if (vPlayerLocalPos.m128_f32[0] < vMonsterLocalPos.m128_f32[0])
	{
		m_pTransformCom->Set_RotationPerSec(XMConvertToRadians(-160.f));
	}

	/*if (XMVector3Equal(vLook, vToPlayerLook))
	{
		*_bState = true;
	}*/

	// 방향을 계속 돌다가, 플레이어를 바라보는 방향벡터와 몬스터의 look 벡터가 비슷할 경우 -> 적 발견!!
	if (Compare_DirVectors(vLook, vToPlayerLook, 0.05f))
	{
		*_bState = true;
	}
}

void CMonster::Collision_ToPlayer()
{
}

_bool CMonster::Collision_ToPlayerBody(MCOLLIDER _eType)
{
	return _bool();
}

_bool CMonster::Collision_ToPlayerSword(MCOLLIDER _eType)
{
	return _bool();
}

_bool CMonster::Collision_ToPlayerShield(MCOLLIDER _eType)
{
	return _bool();
}

void CMonster::Invalidate_ColliderTransformationMatrix()
{
}

_bool CMonster::Compare_DirVectors(const _vector& vec1, const _vector& vec2, _float tolerance)
{
	_vector vNorVec1 = XMVector3Normalize(vec1);
	_vector vNorVec2 = XMVector3Normalize(vec2);

	_float fDotProduct = XMVectorGetX(XMVector3Dot(vNorVec1, vNorVec2));

	// dotProduct가 1에 가까우면 두 벡터의 방향이 유사함을 의미
	return fDotProduct > (1.0f - tolerance);
}

void CMonster::Free()
{
	__super::Free();
}
