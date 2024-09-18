#include "Picking.h"

// Engine
#include "Mesh.h"
#include "Layer.h"
#include "Model.h"
#include "GameObject.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(const GRAPHIC_DESC& _GraphicDesc)
{
	// Init Graphic_Desc
	{
		m_GraphicDesc.hWnd = _GraphicDesc.hWnd;
		m_GraphicDesc.iWinSizeX = _GraphicDesc.iWinSizeX;
		m_GraphicDesc.iWinSizeY = _GraphicDesc.iWinSizeY;
		m_GraphicDesc.bIsWindowed = _GraphicDesc.bIsWindowed;
	}

	return S_OK;
}

_bool CPicking::Mouse_Picking(const _matrix& _Proj, const _matrix& _ViewInverse, const _matrix& _WorldMatrix, 
	_float3* _vPickLocalPos, _float3* _vPickWorldPos, _float* _Dis,
	const CModel* _pModel, CGameObject** _pGameObj, _vector* _vDir, _vector* _vOrigin)
{
	POINT		tMouse;
	{
		GetCursorPos(&tMouse);
		ScreenToClient(m_GraphicDesc.hWnd, &tMouse);
	}

	_float2		vMousePos;
	ZeroMemory(&vMousePos, sizeof(_float2));
	{
		vMousePos.x = (2.f * tMouse.x / m_GraphicDesc.iWinSizeX - 1.f) / _Proj.r[0].m128_f32[0];
		vMousePos.y = (-2.f * tMouse.y / m_GraphicDesc.iWinSizeY + 1.f) / _Proj.r[1].m128_f32[1];
	}

	// 뷰스페이스
	_vector rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector rayDir = XMVectorSet(vMousePos.x, vMousePos.y, 1.f, 0.f);

	_matrix W = _WorldMatrix;
	_vector Determinant = XMMatrixDeterminant(W);
	_matrix invWorld = XMMatrixInverse(&Determinant, W);

	_matrix toLocal = XMMatrixMultiply(_ViewInverse, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	rayDir = XMVector3Normalize(rayDir);

	_int iPickedTriangle = -1;
	float tmin = 100000.f;

	_matrix WorldMatrix;
	CGameObject* pGameObj = { nullptr };

	if (nullptr == _pModel)
	{
		CLayer* pLayer = m_pGameInstance->Find_Layer(3, TEXT("Layer_Obj"));

		for (const auto& element : pLayer->Get_List())
		{
			CModel* pModel = dynamic_cast<CModel*>(element->Get_Component(TEXT("Com_Model")));

			for (size_t i = 0; i < pModel->Get_NumMeshes(); i++)
			{
				m_pMesh = pModel->Get_VecMeshes()[i];

				// For Picking
				{
					m_pVertices = new VTXMESH[m_pMesh->Get_NumVertices()];
					for (size_t i = 0; i < m_pMesh->Get_NumVertices(); i++)
					{
						m_pVertices[i] = m_pMesh->Get_Vertices()[i];
					}

					m_pIndices = new _uint[m_pMesh->Get_NumIndices()];
					for (size_t i = 0; i < m_pMesh->Get_NumIndices(); i++)
					{
						m_pIndices[i] = m_pMesh->Get_Indices()[i];
					}
				}

				for (_uint i = 0; i < m_pMesh->Get_NumIndices() / 3; ++i) // 삼각형마다 3개의 인덱스를 사용하므로 인덱스를 3씩 증가시킵니다.
				{
					_uint iIndices[3] = {
						m_pIndices[i * 3],
						m_pIndices[i * 3 + 1],
						m_pIndices[i * 3 + 2]
					};
					_uint v = m_pMesh->Get_NumIndices();
					_vector v0 = XMLoadFloat3(&m_pVertices[iIndices[0]].vPosition);
					_vector v1 = XMLoadFloat3(&m_pVertices[iIndices[1]].vPosition);
					_vector v2 = XMLoadFloat3(&m_pVertices[iIndices[2]].vPosition);

					float t = 0.f;

					if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
					{
						if (t < tmin)
						{
							tmin = t;
							iPickedTriangle = i;
							*_pGameObj = element;
							WorldMatrix = dynamic_cast<CTransform*>(element->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
						}
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < _pModel->Get_NumMeshes(); i++)
		{
			m_pMesh = _pModel->Get_VecMeshes()[i];

			// For Picking
			{
				m_pVertices = new VTXMESH[m_pMesh->Get_NumVertices()];
				for (size_t i = 0; i < m_pMesh->Get_NumVertices(); i++)
				{
					m_pVertices[i] = m_pMesh->Get_Vertices()[i];
				}

				m_pIndices = new _uint[m_pMesh->Get_NumIndices()];
				for (size_t i = 0; i < m_pMesh->Get_NumIndices(); i++)
				{
					m_pIndices[i] = m_pMesh->Get_Indices()[i];
				}
			}

			for (_uint i = 0; i < m_pMesh->Get_NumIndices() / 3; ++i) // 삼각형마다 3개의 인덱스를 사용하므로 인덱스를 3씩 증가시킵니다.
			{
				_uint iIndices[3] = {
					m_pIndices[i * 3],
					m_pIndices[i * 3 + 1],
					m_pIndices[i * 3 + 2]
				};
				_uint v = m_pMesh->Get_NumIndices();
				_vector v0 = XMLoadFloat3(&m_pVertices[iIndices[0]].vPosition);
				_vector v1 = XMLoadFloat3(&m_pVertices[iIndices[1]].vPosition);
				_vector v2 = XMLoadFloat3(&m_pVertices[iIndices[2]].vPosition);

				float t = 0.f;

				if (DirectX::TriangleTests::Intersects(rayOrigin, rayDir, v0, v1, v2, t))
				{
					if (t < tmin)
					{
						tmin = t;
						*_Dis = tmin;
						iPickedTriangle = i;
					}
				}
			}
		}
	}


	if (iPickedTriangle == -1)
		return false;

	_vector vecPickLocalPos = rayDir * tmin + rayOrigin;
	XMStoreFloat3(_vPickLocalPos, vecPickLocalPos);

	//if (nullptr == _pModel)
	//{
	//	_vector vPickWorldPos = XMVector3TransformCoord(vecPickLocalPos, WorldMatrix);
	//	XMStoreFloat3(_vPickWorldPos, vPickWorldPos);
	//}
	//else
	//{
		_vector vPickWorldPos = XMVector3TransformCoord(vecPickLocalPos, _WorldMatrix);
		XMStoreFloat3(_vPickWorldPos, vPickWorldPos);
	//}

	*_vDir = rayDir;
	*_vOrigin = rayOrigin;

	return true;
}

CPicking* CPicking::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const GRAPHIC_DESC& _GraphicDesc)
{
	CPicking* pInstance = new CPicking(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CPicking");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pIndices);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
