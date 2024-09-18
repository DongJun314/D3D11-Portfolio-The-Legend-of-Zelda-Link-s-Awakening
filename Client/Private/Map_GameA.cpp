#include "stdafx.h"
#include "Map_GameA.h"

// Engine
#include "Model.h"
#include "Mesh_Terrain.h"

#include "LandObject.h"
#include "Layer.h"
#include "Player.h"

// For Binary
#include <fstream>

CMap_GameA::CMap_GameA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMap_GameA::CMap_GameA(const CMap_GameA& _rhs)
	: CGameObject(_rhs)
{
}

_uint CMap_GameA::Get_Stage_HitCount(STAGE _eStage)
{
	if (_eStage > STAGE_4)
		return E_FAIL;

	switch (_eStage)
	{
	case STAGE_1:
		return m_iStage1_HitCount;

	case STAGE_2:
		return m_iStage2_HitCount;

	case STAGE_3:
		return m_iStage3_HitCount;

	case STAGE_4:
		return m_iStage4_HitCount;
	}

	return S_OK;
}

_bool CMap_GameA::Get_StageDataOnce(STAGE _eStage)
{
	switch (_eStage)
	{
	case STAGE_1:
		return m_bStage1DataOnce;

	case STAGE_2:
		return m_bStage2DataOnce;

	case STAGE_3:
		return m_bStage3DataOnce;

	case STAGE_4:
		return m_bStage4DataOnce;
	}

	return false;
}

HRESULT CMap_GameA::Add_HitCount(STAGE _eStage)
{
	if (_eStage > STAGE_4)
		return E_FAIL;

	switch (_eStage)
	{
	case STAGE_1:
		m_iStage1_HitCount++;
		break;

	case STAGE_2:
		m_iStage2_HitCount++;
		break;

	case STAGE_3:
		m_iStage3_HitCount++;
		break;

	case STAGE_4:
		m_iStage4_HitCount++;
		break;
	}

	return S_OK;
}

HRESULT CMap_GameA::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap_GameA::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;


	_vector vecPos = { 0.f, 0.f, 2000.f, 1.0f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPos);

	return S_OK;
}

void CMap_GameA::Priority_Tick(_float _fTimeDelta)
{
	// 여기 약간 느낌이 결국에서는 
	CLandObject::LANDOBJ_DESC LandObjDesc{};
	{
		LandObjDesc.pNavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecCells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation")))->Get_VecCell();

		// Mini GameA
		LandObjDesc.pGameA_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecGameA_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_MiniGameA")))->Get_VecCell();

		// Mini GameB
		/*LandObjDesc.pGameB_NavTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, L"Layer_Nav", g_strTransformTag));
		LandObjDesc.vecGameB_Cells = dynamic_cast<CNavigation*>(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Nav"), TEXT("Com_Navigation_MiniGameB")))->Get_VecCell();*/
	}

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return;

	if (pPlayer->Get_MiniGameA() && !m_bLockOnce)
	{
		m_bStage1DataOnce = true;
		m_bLockOnce = true;
	}

	if (m_bStage1DataOnce)
	{
		for (size_t i = 0; i < 15; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), TEXT("Prototype_GameObject_MiniGameA_Albatoss"), &LandObjDesc)))
				return;

			/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage1"), TEXT("Prototype_GameObject_MiniGameA_Albatoss"), &LandObjDesc)))
				return;*/
		}

		m_bStage1DataOnce = false;
	}

	if (m_bStage2DataOnce)
	{
		for (size_t i = 0; i < 15; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), TEXT("Prototype_GameObject_MiniGameA_BonePutter"), &LandObjDesc)))
				return;

			/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage2"), TEXT("Prototype_GameObject_MiniGameA_BonePutter"), &LandObjDesc)))
				return;*/
		}

		m_bStage2DataOnce = false;
	}

	if (m_bStage3DataOnce)
	{
		for (size_t i = 0; i < 15; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), TEXT("Prototype_GameObject_MiniGameA_FlyingOctorok"), &LandObjDesc)))
				return;

			/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage3"), TEXT("Prototype_GameObject_MiniGameA_FlyingOctorok"), &LandObjDesc)))
				return;*/
		}

		m_bStage3DataOnce = false;
	}

	if (m_bStage4DataOnce)
	{
		for (size_t i = 0; i < 15; i++)
		{
			if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame"), TEXT("Prototype_GameObject_MiniGameA_Piccolo"), &LandObjDesc)))
				return;

			/*if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage4"), TEXT("Prototype_GameObject_MiniGameA_Piccolo"), &LandObjDesc)))
				return;*/
		}

		m_bStage4DataOnce = false;
	}

}

void CMap_GameA::Tick(_float _fTimeDelta)
{
	
}

void CMap_GameA::Late_Tick(_float _fTimeDelta)
{
	CLayer* pStage = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame")));
	if (pStage == nullptr)
		return;

	/*CLayer* pStage1 = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage1")));
	if (pStage1 == nullptr)
		return;

	CLayer* pStage2 = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage2")));
	if (pStage2 == nullptr)
		return;

	CLayer* pStage3 = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage3")));
	if (pStage3 == nullptr)
		return;

	CLayer* pStage4 = dynamic_cast<CLayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_MiniGame_Stage4")));
	if (pStage4 == nullptr)
		return;*/

	// 스테이지 1
	if (m_iStage1_HitCount == 4)
	{
		m_iStage1_HitCount = 0;
		m_bStage2DataOnce = true; 
	}

	// 스테이지 2
	if (m_iStage2_HitCount == 5)
	{
		m_iStage2_HitCount = 0;
		m_bStage3DataOnce = true;
	}

	// 스테이지 3
	if (m_iStage3_HitCount == 6)
	{
		m_iStage3_HitCount = 0;
		m_bStage4DataOnce = true;
	}

	// 스테이지 4
	if (m_iStage4_HitCount == 7)
	{
		m_iStage4_HitCount = 0;
	}


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMap_GameA::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CMap_GameA::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MiniGameA"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_GameA::Bind_ShaderResources()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	_float4 CamPos = m_pGameInstance->Get_CamPosition();

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CMap_GameA* CMap_GameA::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMap_GameA* pInstance = new CMap_GameA(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMap_GameA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap_GameA::Clone(void* _pArg, _uint _iLevel)
{
	CMap_GameA* pInstance = new CMap_GameA(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMap_GameA");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_GameA::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
