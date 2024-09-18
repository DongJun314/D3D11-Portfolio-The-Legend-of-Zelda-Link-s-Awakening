#pragma once

#include "VIBuffer.h"

// Engine
#include "Model.h"

/* CMesh : ���� �����ϴ� �ϳ��� �޽ø� ���� ������ �ε����� �����Ѵ�. */
BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMesh(const CMesh& _rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	_uint Get_NumBones() const {
		return m_iNumBones;
	}

	_uint Get_NumVertices() const {
		return m_iNumVertices;
	}

	_uint Get_NumIndices() const {
		return m_iNumIndices;
	}

	_uint* Get_Indices() const {
		return m_pIndices; 
	}

	VTXMESH* Get_Vertices() const { 
		return m_pVertices; 
	}

public:
	virtual HRESULT Initialize_Prototype(std::ifstream* _pReadBinary, CModel* _pModel, CModel::TYPE _eType, _fmatrix _PivotMatrix);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	void SetUp_BoneMatrices(_float4x4* _pBoneMatrices, vector<class CBone*>& _vecBones);

public:
	HRESULT Ready_Vertex_Buffer_NonAnim(std::ifstream* _pReadBinary, _fmatrix _PivotMatrix);
	
	HRESULT Ready_Vertex_Buffer_Anim(std::ifstream* _pReadBinary, CModel* _pModel);

public:
	static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, std::ifstream* _pReadBinary, CModel* _pModel, CModel::TYPE _eType, _fmatrix _PivotMatrix);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_char m_szName[MAX_PATH] = "";

	_uint m_iMaterialIndex = { 0 };
	_uint m_iNumBones = { 0 };

	_float4x4 m_PivotMatrix = {};

	vector<_uint> m_vecBoneIndices; // �޽ú��� �����ؼ� ��Ƶ� ������ �ε���, ����
	vector<_float4x4> m_vecOffsetMatrices;

private:
	_uint* m_pIndices = { nullptr };	// Picking�� ���� ����ü
	VTXMESH* m_pVertices = { nullptr };	// Picking�� ���� ����ü


};

END