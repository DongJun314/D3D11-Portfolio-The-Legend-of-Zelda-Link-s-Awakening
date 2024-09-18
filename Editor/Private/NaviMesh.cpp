#include "pch.h"
#include "NaviMesh.h"

// Engine
#include "Model.h"
#include "Layer.h"
#include "Mesh_Terrain.h"
#include "ImGui_Manager.h"

//bool g_bIsPicked;

CNaviMesh::CNaviMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CNaviMesh::CNaviMesh(const CNaviMesh& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CNaviMesh::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CNaviMesh::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CNaviMesh::Priority_Tick(_float _fTimeDelta)
{
}

void CNaviMesh::Tick(_float _fTimeDelta)
{
	// NaviMesh
	if (m_pGameInstance->Key_Down('N'))
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

		m_vecPoints.push_back(m_vPickWorldPos);

		if (m_vecPoints.size() == 3)
		{
			_float3 vPoints[3] = {};

			for (size_t i = 0; i < m_vecPoints.size(); i++)
			{
				m_iCount++;
				vPoints[i] = m_vecPoints[i];
			}
			m_vecPoints.clear();

			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			CELLS tCells;
			{
				tCells.A = vPoints[0];
				tCells.B = vPoints[1];
				tCells.C = vPoints[2];
			}

			Merge_Points(&tCells);

			m_vecCells.push_back(tCells);

			vPoints[0] = tCells.A;
			vPoints[1] = tCells.B;
			vPoints[2] = tCells.C;
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			sizeWStr = to_wstring(m_iCount);

			/* For.Prototype_Component_Navigation */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation") + sizeWStr,
				CNavigation::Create(m_pDevice, m_pContext, TEXT("..."), vPoints))))
				return;

			/* Com_Navigation */
			CNavigation* m_pNavigationCom;
			if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation") + sizeWStr,
				TEXT("Com_Navigation") + sizeWStr, reinterpret_cast<CComponent**>(&m_pNavigationCom))))
				return;

			m_vecNaviCom.push_back(m_pNavigationCom);
		}

	}
			

	if (m_pGameInstance->Key_Pressing(VK_CONTROL) && m_pGameInstance->Key_Down('B'))
	{
		//CComponent* pComponent = m_pGameInstance->Find_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation") + sizeWStr);
		//dynamic_cast<CNavigation*>(pComponent)->Clear_Cell();

		//__super::Clear_Component(TEXT("Com_Navigation") + sizeWStr);
		m_vecNaviCom.pop_back();
		m_vecCells.pop_back();
	}

	if (m_pGameInstance->Key_Pressing(VK_CONTROL) && m_pGameInstance->Key_Down('S'))
	{
		_ulong dwByte = { 0 };

		HANDLE hFile = CreateFile(TEXT("../../Client/Bin/DataFiles/NaviMesh_Test.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		for (size_t i = 0; i < m_vecCells.size(); i++)
		{
			_float3 vPoints[3] = {};

			vPoints[0] = m_vecCells[i].A;
			vPoints[1] = m_vecCells[i].B;
			vPoints[2] = m_vecCells[i].C;

			::WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		}

		::CloseHandle(hFile);
	}
	if (m_vecNaviCom.size() != 0)
	{
		for (size_t i = 0; i < m_vecNaviCom.size(); i++)
		{
			m_vecNaviCom[i]->Tick(XMMatrixIdentity());
		}
	}
}

void CNaviMesh::Late_Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CNaviMesh::Render()
{
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

HRESULT CNaviMesh::Adjust_to_Clockwise(_float3* _pPoints)
{
	_vector vPointA = XMLoadFloat3(&_pPoints[0]);
	_vector vPointB = XMLoadFloat3(&_pPoints[1]);
	_vector vPointC = XMLoadFloat3(&_pPoints[2]);

	_vector vDirAB = XMVectorSubtract(vPointB, vPointA);  // AB 방향 벡터
	_vector vDirAC = XMVectorSubtract(vPointC, vPointA);  // AC 방향 벡터

	// 두 벡터의 외적 계산
	_vector vCross = XMVector3Cross(vDirAB, vDirAC);

	_float vCrossY = XMVectorGetY(vCross);

	if (vCrossY < 0)
	{
		XMStoreFloat3(&_pPoints[1], vPointC);
		XMStoreFloat3(&_pPoints[2], vPointB);
	}

	return S_OK;
}

HRESULT CNaviMesh::Merge_Points(CELLS* _tCells)
{
	for (auto& pSourCell : m_vecCells)
	{
		if (FAILED(Compare_Points_Dis(pSourCell.A, _tCells)))
			return E_FAIL;

		if (FAILED(Compare_Points_Dis(pSourCell.B, _tCells)))
			return E_FAIL;

		if (FAILED(Compare_Points_Dis(pSourCell.C, _tCells)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNaviMesh::Compare_Points_Dis(const _float3& _pSourPoint, CELLS* _tCells)
{
	// 거리AA
	_float fDisAX = fabs(_pSourPoint.x - _tCells->A.x);
	_float fDisAY = fabs(_pSourPoint.y - _tCells->A.y);
	_float fDisAZ = fabs(_pSourPoint.z - _tCells->A.z);

	_float fDisA = fDisAX + fDisAY + fDisAZ;

	if (fDisA < 0.5f)
	{
		_tCells->A = _pSourPoint;
	}

	// 거리 AB
	_float fDisBX = fabs(_pSourPoint.x - _tCells->B.x);
	_float fDisBY = fabs(_pSourPoint.y - _tCells->B.y);
	_float fDisBZ = fabs(_pSourPoint.z - _tCells->B.z);

	_float fDisB = fDisBX + fDisBY + fDisBZ;

	if (fDisB < 0.5f)
	{
		_tCells->B = _pSourPoint;
	}

	// 거리 AC
	_float fDisCX = fabs(_pSourPoint.x - _tCells->C.x);
	_float fDisCY = fabs(_pSourPoint.y - _tCells->C.y);
	_float fDisCZ = fabs(_pSourPoint.z - _tCells->C.z);

	_float fDisC = fDisCX + fDisCY + fDisCZ;

	if (fDisC < 0.5f)
	{
		_tCells->C = _pSourPoint;
	}

	return S_OK;
}

CNaviMesh* CNaviMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNaviMesh* pInstance = new CNaviMesh(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNaviMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaviMesh::Clone(void* _pArg, _uint _iLevel)
{
	CNaviMesh* pInstance = new CNaviMesh(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CNaviMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNaviMesh::Free()
{
	__super::Free();
}
