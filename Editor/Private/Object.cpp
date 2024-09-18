#include "pch.h"
#include "Object.h"

// Engine
#include "Model.h"
#include "Layer.h"
#include "Mesh_Terrain.h"
#include "ImGui_Manager.h"

//bool g_bIsPicked;

CObject::CObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CObject::CObject(const CObject& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CObject::Initialize_Prototype()
{

	return S_OK; 
}

HRESULT CObject::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CObject::Priority_Tick(_float _fTimeDelta)
{
}

void CObject::Tick(_float _fTimeDelta)
{
	
	///*if (GetKeyState(VK_RBUTTON) & 0x8000)
	//{
	//	*//*if (m_pGameInstance->Key_Down(VK_RBUTTON))
	//	{
	//	CGameObject* pGaemObject = { nullptr };

	//	if (m_pGameInstance->Mouse_Picking(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
	//		m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW),
	//		m_pTransformCom->Get_WorldMatrix(),
	//		&m_vPickLocalPos, &m_vPickWorldPos, nullptr, pGaemObject))
	//	{*/

	if (m_pGameInstance->Key_Down(VK_RBUTTON))
	{
		_float tmin = 100000.f;
		_vector vDir;
		_vector vOrigin;
		CGameObject* pGaemObject = nullptr;


		CLayer* pLayer = m_pGameInstance->Find_Layer(3, TEXT("Layer_Obj"));
		if (nullptr == pLayer)
			return;

		for (const auto& element : pLayer->Get_List())
		{
			float t = 0.f;
			CModel* pModel = dynamic_cast<CModel*>(element->Get_Component(TEXT("Com_Model")));

			if (m_pGameInstance->Mouse_Picking(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ),
				m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW),
				dynamic_cast<CTransform*>(element->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix(),
				&m_vPickLocalPos, &m_vPickWorldPos, &t, pModel, &pGaemObject, &vDir, &vOrigin))
			{
				m_bIsPicked = true;

				if (t < tmin)
				{
					tmin = t;
					pGaemObject = element;

					_vector vecPickLocalPos = vDir * tmin + vOrigin;
					XMStoreFloat3(&m_vPickLocalPos, vecPickLocalPos);
					
					_vector vPickWorldPos = XMVector3TransformCoord(vecPickLocalPos, dynamic_cast<CTransform*>(element->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix());
					XMStoreFloat3(&m_vPickWorldPos, vPickWorldPos);
				}
			}
		}

		if (tmin != 100000.f)
		{
			CImGui_Manager::GetInstance()->Set_CurOBJ(dynamic_cast<class CGameObject*> (pGaemObject));

			if (CImGui_Manager::GetInstance()->Get_CurOBJ()->Get_KeyValue() == TEXT("Nothing"))
			{
				// string으로 되어 있는 file path를 file Name을 얻어온다.
				char szFileName[MAX_PATH];
				string strFilePath = CImGui_Manager::GetInstance()->Get_strFilePath();
				_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
				string strFileName = szFileName;

				// string을 wstring으로 바꿉니다
				wstring wstrFileName;
				wstrFileName.assign(strFileName.begin(), strFileName.end());

				wstring wstrKeyValue; // 맵에 집어 넣을 다른 키값(중복키값 허용x)
				CImGui_Manager::GetInstance()->Set_OBJ(wstrFileName, dynamic_cast<class CGameObject*> (pGaemObject), &wstrKeyValue);
				CImGui_Manager::GetInstance()->Get_CurOBJ()->Set_KeyValue(wstrKeyValue);
			}
		}
		else
		{
			CImGui_Manager::GetInstance()->Set_CurOBJ(dynamic_cast<class CGameObject*> (this));
			if (CImGui_Manager::GetInstance()->Get_CurOBJ()->Get_KeyValue() == TEXT("Nothing"))
			{
				// string으로 되어 있는 file path를 file Name을 얻어온다.
				char szFileName[MAX_PATH];
				string strFilePath = CImGui_Manager::GetInstance()->Get_strFilePath();
				_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
				string strFileName = szFileName;

				// string을 wstring으로 바꿉니다
				wstring wstrFileName;
				wstrFileName.assign(strFileName.begin(), strFileName.end());

				wstring wstrKeyValue; // 맵에 집어 넣을 다른 키값(중복키값 허용x)
				CImGui_Manager::GetInstance()->Set_OBJ(wstrFileName, dynamic_cast<class CGameObject*> (this), &wstrKeyValue);
				CImGui_Manager::GetInstance()->Get_CurOBJ()->Set_KeyValue(wstrKeyValue);
			}
		}
	}

	//if (m_vecNaviCom.size() != 0)
	//{
	//	for (size_t i = 0; i < m_vecNaviCom.size(); i++)
	//	{
	//		m_vecNaviCom[i]->Tick(m_pTransformCom->Get_WorldMatrix());
	//	}
	//}

}

void CObject::Late_Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CObject::Render()
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
//	
//#ifdef _DEBUG
//	if (m_vecNaviCom.size() != 0)
//	{
//		for (size_t i = 0; i < m_vecNaviCom.size(); i++)
//		{
//			m_vecNaviCom[i]->Render();
//		}
//	}
//#endif

	return S_OK;
}


HRESULT CObject::Add_Component()
{
	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	char szFileName[MAX_PATH];
	string strFilePath = CImGui_Manager::GetInstance()->Get_strFilePath();
	_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	string strFileName = szFileName;

	wstring wstrFileName;
	wstrFileName.assign(strFileName.begin(), strFileName.end());

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Object" + wstrFileName),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CObject::Bind_ShaderResources()
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

CObject* CObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CObject* pInstance = new CObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CObject::Clone(void* _pArg, _uint _iLevel)
{
	CObject* pInstance = new CObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
