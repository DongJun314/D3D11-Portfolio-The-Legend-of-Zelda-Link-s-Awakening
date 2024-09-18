#include "pch.h"
#include "MapObject.h"

// Editor
#include "ImGui_Manager.h"

// Engine
#include "Model.h"
#include "Mesh_Terrain.h"

// For Binary
#include <fstream>

CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{
	wstring* wstrFileName = (wstring*)_pArg;
	{
 		m_wstrFileName = *wstrFileName;
		wstrFileName = nullptr;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CMapObject::Priority_Tick(_float _fTimeDelta)
{
}

void CMapObject::Tick(_float _fTimeDelta)
{
	//// 피킹
	//if (m_pGameInstance->Key_Down(VK_RBUTTON))
	//{
	//	CGameObject* pGameObject = { nullptr };

	//	if (m_pGameInstance->Mouse_Picking(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
	//		m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW),
	//		m_pTransformCom->Get_WorldMatrix(),
	//		&m_vPickLocalPos, &m_vPickWorldPos, m_pModelCom, pGameObject))
	//	{
	//		m_bIsPicked = true;

	//		if(nullptr != pGameObject)
	//			CImGui_Manager::GetInstance()->Set_CurOBJ(dynamic_cast<class CGameObject*> (pGameObject));
	//		else
	//			CImGui_Manager::GetInstance()->Set_CurOBJ(dynamic_cast<class CGameObject*> (this));

	//		if (CImGui_Manager::GetInstance()->Get_CurOBJ()->Get_KeyValue() == TEXT("Nothing"))
	//		{
	//			// string으로 되어 있는 file path를 file Name을 얻어온다.
	//			char szFileName[MAX_PATH];
	//			string strFilePath = CImGui_Manager::GetInstance()->Get_strFilePath();
	//			_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	//			string strFileName = szFileName;

	//			// string을 wstring으로 바꿉니다
	//			wstring wstrFileName;
	//			wstrFileName.assign(strFileName.begin(), strFileName.end());

	//			wstring wstrKeyValue; // 맵에 집어 넣을 다른 키값(중복키값 허용x)
	//			CImGui_Manager::GetInstance()->Set_OBJ(wstrFileName, dynamic_cast<class CGameObject*> (this), &wstrKeyValue);
	//			CImGui_Manager::GetInstance()->Get_CurOBJ()->Set_KeyValue(wstrKeyValue);
	//		}
	//	}
	//}


	//// NaviMesh
	//if (m_pGameInstance->Key_Down('N'))
	//{
	//	if (m_pGameInstance->Mouse_Picking(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
	//		m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW),
	//		m_pTransformCom->Get_WorldMatrix(),
	//		&m_vPickLocalPos, &m_vPickWorldPos, m_pModelCom))
	//	{
	//		m_vecPoints.push_back(m_vPickLocalPos);

	//		if (m_vecPoints.size() == 3)
	//		{
	//			_float3 vPoints[3] = {};
	//			for (size_t i = 0; i < m_vecPoints.size(); i++)
	//			{
	//				vPoints[i] = m_vecPoints[i];
	//			}

	//			m_vecPoints.clear();

	//			wstring sizeWStr = to_wstring(m_iCount);

	//			/* For.Prototype_Component_Navigation */
	//			m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation") + sizeWStr,
	//				CNavigation::Create(m_pDevice, m_pContext, TEXT("..."), vPoints));

	//			m_vPoints[m_iCount++] = vPoints[0];
	//			m_vPoints[m_iCount++] = vPoints[1];
	//			m_vPoints[m_iCount++] = vPoints[2];


	//			/* Com_Navigation */
	//			CNavigation* m_pNavigationCom;
	//			__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation") + sizeWStr,
	//				TEXT("Com_Navigation") + sizeWStr, reinterpret_cast<CComponent**>(&m_pNavigationCom));

	//			m_vecNaviCom.push_back(m_pNavigationCom);
	//		}
	//	}
	//}

	if (m_vecNaviCom.size() != 0)
	{
		for (size_t i = 0; i < m_vecNaviCom.size(); i++)
		{
			m_vecNaviCom[i]->Tick(m_pTransformCom->Get_WorldMatrix());
		}
	}
}

void CMapObject::Late_Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CMapObject::Render()
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

#ifdef _DEBUG
	if (m_vecNaviCom.size() != 0)
	{
		for (size_t i = 0; i < m_vecNaviCom.size(); i++)
		{
			m_vecNaviCom[i]->Render();
		}
	}
#endif

	return S_OK;
}


HRESULT CMapObject::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Object_" + m_wstrFileName),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
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

CMapObject* CMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMapObject* pInstance = new CMapObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg, _uint _iLevel)
{
	CMapObject* pInstance = new CMapObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
