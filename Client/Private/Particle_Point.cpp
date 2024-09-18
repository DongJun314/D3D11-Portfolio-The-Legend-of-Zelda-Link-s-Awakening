#include "stdafx.h"
#include "Particle_Point.h"

#include "Player.h"

CParticle_Point::CParticle_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CParticle_Point::CParticle_Point(const CParticle_Point& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CParticle_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Point::Initialize(void* _pArg)
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	{
		GameObjectDesc.iGameObjectData = 100;
		GameObjectDesc.fSpeedPerSec = 10.0f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.0f);
	}

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CParticle_Point::Priority_Tick(_float _fTimeDelta)
{
	Handle_ParticleType(_fTimeDelta);
}

void CParticle_Point::Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

void CParticle_Point::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CParticle_Point::Render()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	// 받아온 카메라 위치 정보 쉐이더에 던지기
	_float4 CamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Point::Handle_ParticleType(_float _fTimeDelta)
{
	if (m_eParticleType == PT_SPREAD)
	{
		//m_pVIBufferCom->Tick_Spread(_fTimeDelta);
		m_pVIBufferCom->Tick_Spread(_fTimeDelta);
	}
	
	if (m_eParticleType == PT_CHARGE)
	{
		m_pVIBufferCom->Tick_Charge(_fTimeDelta);
	}

	return S_OK;
}

void CParticle_Point::Reset_Particle_LifeTime(_float _fTimeDelta)
{
	m_pVIBufferCom->Tick_Reset(_fTimeDelta);
}

HRESULT CParticle_Point::Add_Component()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			InstanceDesc{};
	{
		InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
		InstanceDesc.vCenter = _float3(0.0f, 0.f, 0.f);
		InstanceDesc.vRange = _float3(12.f, 12.f, 12.f);
		InstanceDesc.vSize = _float2(0.11f, 0.11f);
		InstanceDesc.vSpeed = _float2(5.f, 15.f);
		InstanceDesc.vLifeTime = _float2(3.f, 5.f);
		InstanceDesc.bIsLoop = false;
		InstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstanceDesc)))
		return E_FAIL;

	//CVIBuffer_Instancing::INSTANCE_DESC			slashInstanceDesc{};
	//{
	//	slashInstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	//	slashInstanceDesc.vCenter = _float3(0.0f, 0.f, 0.f);
	//	slashInstanceDesc.vRange = _float3(12.f, 12.f, 12.f);
	//	slashInstanceDesc.vSize = _float2(0.11f, 0.11f);
	//	slashInstanceDesc.vSpeed = _float2(5.f, 15.f);
	//	slashInstanceDesc.vLifeTime = _float2(3.f, 5.f);
	//	slashInstanceDesc.bIsLoop = false;
	//	slashInstanceDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	//}

	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
	//	TEXT("Com_Slash_VIBuffer"), reinterpret_cast<CComponent**>(&m_pSlash_VIBufferCom), &slashInstanceDesc)))
	//	return E_FAIL;

	return S_OK;
}

CParticle_Point* CParticle_Point::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CParticle_Point* pInstance = new CParticle_Point(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticle_Point::Clone(void* _pArg, _uint _iLevel)
{
	CParticle_Point* pInstance = new CParticle_Point(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Point::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
