#include "Model.h"

// Engine
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

// For Reading Binary Files
#include <fstream>

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CModel::CModel(const CModel& _rhs)
	: CComponent(_rhs)
	, m_iNumMeshes(_rhs.m_iNumMeshes)
	, m_PivotMatrix(_rhs.m_PivotMatrix)
	, m_iNumMaterials(_rhs.m_iNumMaterials)
	, m_vecMaterials(_rhs.m_vecMaterials)
	, m_vecMeshes(_rhs.m_vecMeshes)
	, m_iNumAnimations(_rhs.m_iNumAnimations)
{
	for (auto& pPrototypeAnimation : _rhs.m_vecAnimations)
		m_vecAnimations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : _rhs.m_vecBones)
		m_vecBones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_vecMaterials)
	{
		for (auto& pTexture : Material.pMtrlTextures)
			Safe_AddRef(pTexture);
	}
}

_int CModel::Get_BoneIndex(const _char* _pBoneName) const
{
	_int iBoneIndex = { -1 };

	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](class CBone* pBone) {

		iBoneIndex++;

		if (false == strcmp(_pBoneName, pBone->Get_BoneName()))
			return true;
		return false;
	});

 	if (iter == m_vecBones.end())
		return -1;

	return iBoneIndex;
}

_float4x4* CModel::Get_CombinedBoneMatrixPtr(const _char* _pBoneName)
{
	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* _pBone)
	{
		if (false == strcmp(_pBone->Get_BoneName(), _pBoneName))
			return true;

		return false;
	});

	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

_float4x4* CModel::Get_TransformationMatrixPtr(const _char* _pBoneName)
{
	auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* _pBone)
		{
			if (false == strcmp(_pBone->Get_BoneName(), _pBoneName))
				return true;

			return false;
		});

	return (*iter)->Get_TransformationMatrixPtr();
}

HRESULT CModel::Initialize_Prototype(TYPE _eModelType, const wstring& _pModelFilePath, _fmatrix _PivotMatrix)
{
	m_eModelType = _eModelType;

	XMStoreFloat4x4(&m_PivotMatrix, _PivotMatrix);

	// 모델의 파일 경로에서 파일 이름만 추출한되, 멤버변수에 저장하기
	// Object Tick함수에서 사용하기 위함
	_tchar tszFileName[MAX_PATH];
	_wsplitpath_s(_pModelFilePath.c_str(), nullptr, 0, nullptr, 0, tszFileName, MAX_PATH, nullptr, 0);
	m_wstrModelFileName = tszFileName;


	HRESULT hr = _eModelType == CModel::TYPE_NONANIM ? Ready_NonAnim_Model(_pModelFilePath, _PivotMatrix) : Ready_Anim_Model(_pModelFilePath);
	if (FAILED(hr))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CModel::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint _iMeshIndex)
{
	m_vecMeshes[_iMeshIndex]->Bind_Buffers();
	m_vecMeshes[_iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Bind_Material_ShaderResource(CShader* _pShader, _uint _iMeshIndex, aiTextureType _eMaterialType, const _char* _pConstantName)
{
	_uint iMaterialIndex = m_vecMeshes[_iMeshIndex]->Get_MaterialIndex();

	if (m_vecMaterials[iMaterialIndex].pMtrlTextures[_eMaterialType])
		m_vecMaterials[iMaterialIndex].pMtrlTextures[_eMaterialType]->Bind_ShaderResource(_pShader, _pConstantName, 0);

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* _pShader, const _char* _pConstantName, _uint _iMeshIndex)
{
	_float4x4 BoneMatrices[256];

	m_vecMeshes[_iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, m_vecBones);

	return _pShader->Bind_Matrices(_pConstantName, BoneMatrices, 256);
}

void CModel::Play_Animation(_float _fTimeDelta, _bool _bIsLoop)
{	
	if (m_iCurrentAnimation != m_iNextAnimation)
			m_vecAnimations[m_iCurrentAnimation]->Invalidate_Linear_TransformationMatrix(_fTimeDelta, m_vecBones, m_vecAnimations[m_iNextAnimation], &m_iCurrentAnimation, &m_iNextAnimation);
	  
 	//* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
	//if (m_iCurrentAnimation == m_iNextAnimation)
	//	m_vecAnimations[m_iCurrentAnimation]->Invalidate_TransformationMatrix(_fTimeDelta, m_vecBones, _bIsLoop);

	/* 현재 애니메이션 상태에 맞게 뼈들의 TransformationMatrix행렬을 갱신한다. */
	if (m_iCurrentAnimation == m_iNextAnimation)
		m_vecAnimations[m_iCurrentAnimation]->Test_Invalidate_TransformationMatrix(_fTimeDelta, m_vecBones, _bIsLoop, &m_iCurrentAnimation, &m_iNextAnimation);

	/* 모든 뼈들의 CombinedTransformationMatrix를 구해준다. */
	for (auto& pBone : m_vecBones)
		pBone->Invalidate_CombinedTransformationMatrix(m_vecBones);
}



HRESULT CModel::Ready_Meshes(std::ifstream* _pReadBinary)
{
	CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, _pReadBinary, this, m_eModelType, XMLoadFloat4x4(&m_PivotMatrix));
	if (nullptr == pMesh)
		return E_FAIL;

	m_vecMeshes.push_back(pMesh);

	return S_OK;
}


HRESULT CModel::Ready_Materials(std::ifstream* _pReadBinary)
{
 	_bool bIsTextureFile{};
	MESH_MATERIAL_DESC 			MaterialDesc = {};

	for (size_t i = 0; i < AiTextureType_TRANSMISSION; i++)
	{
		_pReadBinary->read(reinterpret_cast<char*>(&bIsTextureFile), sizeof(_bool));

		if (false == bIsTextureFile)
			continue;

		size_t szLength;
		_pReadBinary->read(reinterpret_cast<char*>(&szLength), sizeof(size_t));

		_char szFullPath[MAX_PATH];
		_pReadBinary->read(reinterpret_cast<char*>(szFullPath), szLength);

		_tchar tszFullPath[MAX_PATH] = TEXT("");
		_int iLength = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, szFullPath, (_int)strlen(szFullPath), NULL, NULL);
		MultiByteToWideChar(CP_ACP, MB_COMPOSITE, szFullPath, MAX_PATH, tszFullPath, MAX_PATH);

		MaterialDesc.pMtrlTextures[i] = CTexture::Create(m_pDevice, m_pContext, tszFullPath, 1);
	}
	m_vecMaterials.push_back(MaterialDesc);

	return S_OK;
#pragma region 설명

	//_char szDrive[MAX_PATH] = "";
	//_char szDirectory[MAX_PATH] = "";

	//_splitpath_s(_pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	//// *Material 정보는 AIScene으로 부터 정보를 얻어온다
	//// *Mesh마다 사용하는 Material이 다를 수 있기에 Material가 여러개 존재 할 수 있다. 꼭 Mesh랑 같아야 하는 것도 아니다
	//m_iNumMaterials = m_pAIScene->mNumMaterials;

	//// 이 포문 자체가 Diffuse도 한장, Ambient도 한장, Specular도 한장을 가져온다
	//for (size_t i = 0; i < m_iNumMaterials; i++)
	//{
	//	MESH_MATERIAL_DESC MaterialDesc = {};

	//	aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i]; // assimp의 이 material를 사용할거에요
	//
	//	for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
	//	{
	//		// *
	//		aiString strTextureFilePath;

	//		// Diffuse의 몇번째 재질? 0번째 재질을 사용할 것이다.
	//		if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
	//			continue;

	//		_char szFileName[MAX_PATH] = "";
	//		_char szExt[MAX_PATH] = "";

	//		_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


	//		_char szTmp[MAX_PATH] = "";

	//		strcpy_s(szTmp, szDrive);
	//		strcat_s(szTmp, szDirectory);
	//		strcat_s(szTmp, szFileName);
	//		strcat_s(szTmp, szExt);

	//		_tchar szFullPath[MAX_PATH] = TEXT("");

	//		// char를 tchar로 바꾸기 위한 함수
	//		MultiByteToWideChar(CP_ACP, 0, szTmp, (_int)strlen(szTmp), szFullPath, MAX_PATH);

	//		MaterialDesc.pMtrlTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, 1);
	//	}

	//	m_vecMaterials.push_back(MaterialDesc);
	//}
#pragma endregion
}



HRESULT CModel::Ready_Bones(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex)
{
 	CBone* pBone = CBone::Create(_szName, _vTransformationMatrix, _vCombinedTransformationMatrix, _iParentBoneIndex, XMLoadFloat4x4(&m_PivotMatrix));
	if (nullptr == pBone)
		return E_FAIL;

  	m_vecBones.push_back(pBone);

	return S_OK;
}

HRESULT CModel::Ready_Anim_Model(const wstring& _pModelFilePath)
{
	std::ifstream ReadBinary(_pModelFilePath, ios::in | ios::binary);

	if (!ReadBinary.is_open())
	{
		MSG_BOX("Failed to Read : Anim Binary File");
		return E_FAIL;
	}


#pragma region Ready_Bones
	// 뼈 개수
	size_t BonesSize;

	ReadBinary.read(reinterpret_cast<char*>(&BonesSize), sizeof(size_t));

	for (size_t i = 0; i < BonesSize; i++)
	{
		_char szName[MAX_PATH] = "";
		ReadBinary.read(reinterpret_cast<char*>(szName), sizeof(szName));

		_float4x4 vTransformationMatrix;
		ReadBinary.read(reinterpret_cast<char*>(&vTransformationMatrix), sizeof(_float4x4));

		_float4x4 vCombinedTransformationMatrix;
		ReadBinary.read(reinterpret_cast<char*>(&vCombinedTransformationMatrix), sizeof(_float4x4));

		_int iParentBoneIndex;
		ReadBinary.read(reinterpret_cast<char*>(&iParentBoneIndex), sizeof(_int));

		// mRootNode의 부모뼈는 없기에 iParentBoneIndex를 -1로 해준다
		if (FAILED(Ready_Bones(szName, vTransformationMatrix, vCombinedTransformationMatrix, iParentBoneIndex)))
			return E_FAIL;
	}
#pragma endregion



	// 메시 개수
	ReadBinary.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));

	// 각 메시 정보
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(Ready_Meshes(&ReadBinary)))
			return E_FAIL;
	}

	// 텍스처 개수
	ReadBinary.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		if (FAILED(Ready_Materials(&ReadBinary)))
			return E_FAIL;
	}


#pragma region Ready_Animations

	ReadBinary.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof(_uint));

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		_char szName[MAX_PATH];
		ReadBinary.read(reinterpret_cast<char*>(szName), sizeof(szName));

		_float Duration;
   		ReadBinary.read(reinterpret_cast<char*>(&Duration), sizeof(_float));

		_float fTickPerSecond;
		ReadBinary.read(reinterpret_cast<char*>(&fTickPerSecond), sizeof(_float));

		_uint iNumChannels;
		ReadBinary.read(reinterpret_cast<char*>(&iNumChannels), sizeof(_uint));

		if (FAILED(Ready_Animation(&ReadBinary, szName, Duration, fTickPerSecond, iNumChannels)))
			return E_FAIL;
	}

#pragma endregion


	ReadBinary.close();

	return S_OK;
}


HRESULT CModel::Ready_NonAnim_Model(const wstring& _pModelFilePath, _fmatrix _PivotMatrix)
{
	std::ifstream ReadBinary(_pModelFilePath, ios::in | ios::binary);

	if (!ReadBinary.is_open())
	{
		MSG_BOX("Failed to Read : NonAnim Binary File");
		return E_FAIL;
	}

	// 메시 개수
	ReadBinary.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));

	// 각 메시 정보
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		if (FAILED(Ready_Meshes(&ReadBinary)))
			return E_FAIL;
	}


	// 텍스처 개수
	ReadBinary.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		if (FAILED(Ready_Materials(&ReadBinary)))
			return E_FAIL;
	}

	ReadBinary.close();
	

	return S_OK;
}

HRESULT CModel::Ready_Animation(std::ifstream* _pReadBinary, const _char* _szName, const _float _fDuration, const _float _fTickPerSecond, const _uint _iNumChannels)
{
	CAnimation* pAnimation = CAnimation::Create(_pReadBinary, _szName, _fDuration, _fTickPerSecond, _iNumChannels, this);
	if (nullptr == pAnimation)
		return E_FAIL;

	m_vecAnimations.push_back(pAnimation);

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, TYPE _eModelType, const wstring& _pModelFilePath, _fmatrix _PivotMatrix)
{
	CModel* pInstance = new CModel(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_eModelType, _pModelFilePath, _PivotMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* _pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_vecAnimations)
		Safe_Release(pAnimation);
	m_vecAnimations.clear();

	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);
	m_vecBones.clear();

	for (auto& Material : m_vecMaterials)
	{
		for (auto& pTexture : Material.pMtrlTextures)
			Safe_Release(pTexture);
	}
	m_vecMaterials.clear();

	for (auto& pMesh : m_vecMeshes)
		Safe_Release(pMesh);
	m_vecMeshes.clear();

	m_Importer.FreeScene();
}
