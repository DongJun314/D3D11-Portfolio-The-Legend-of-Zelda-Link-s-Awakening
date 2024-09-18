#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const _char* Get_BoneName() const {
		return m_szName;
	}

	const _int Get_ParentBoneIndex() const {
		return m_iParentBoneIndex;
	}

	_float4x4 Get_TransformationMatrix() const {
		return m_TransformationMatrix;
	}

	_float4x4* Get_TransformationMatrixPtr() {
		return &m_TransformationMatrix;
	}

	_float4x4 Get_CombinedTransformationMatrix() const {
		return m_CombinedTransformationMatrix;
	}

	// 여기서 Combined Transformation Matrix의 포인터를 반환하는 이유는, 매번 바뀔때 마다 뼈들의 정도를 받는게 싫어서, 참조형태로 한번만 받아서, 나중에 바뀐값을 알기 위해서이다.
	_float4x4* Get_CombinedTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	void Set_TranslationMatrix(_fmatrix _TranslationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, _TranslationMatrix);
	}

public:
	HRESULT Initialize(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex, _fmatrix _PivotMatrix);

public:
	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& _vecBones);

public:
	static CBone* Create(const _char* _szName, const _float4x4 _vTransformationMatrix, const _float4x4 _vCombinedTransformationMatrix, _int _iParentBoneIndex, _fmatrix _PivotMatrix);
	CBone* Clone();
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_char m_szName[MAX_PATH] = "";
	_int m_iParentBoneIndex = { 0 };
	_float4x4 m_TransformationMatrix;
	_float4x4 m_CombinedTransformationMatrix;


};

END