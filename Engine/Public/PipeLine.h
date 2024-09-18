#pragma once

#include "Base.h"

/* 뷰, 투영행렬을 보관한다. */
/* 뷰, 투영행렬을 리턴한다. */
/* 뷰, 투영행렬의 역행렬을 구하고 보관하낟.. */
/* 카메라 월드위치를 구하여 보관한다. */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(D3DTRANSFORMSTATE _eState, _fmatrix _fTransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix[_eState], _fTransformMatrix);
	}

	_float4 Get_CamPosition() {
		return m_vCamPosition;
	}

	_float4x4 Get_Transform_Float4x4(D3DTRANSFORMSTATE _eState) {
		return m_TransformMatrix[_eState];
	}

	_float4x4 Get_Transform_Float4x4_Inverse(D3DTRANSFORMSTATE _eState) {
		return m_TransformMatrix_Inverse[_eState];
	}

	_matrix Get_Transform_Matrix(D3DTRANSFORMSTATE _eState) {
		return XMLoadFloat4x4(&m_TransformMatrix[_eState]);
	}

	_matrix Get_Transform_Matrix_Inverse(D3DTRANSFORMSTATE _eState) {
		return XMLoadFloat4x4(&m_TransformMatrix_Inverse[_eState]);
	}

public:
	void Tick();

public:
	static CPipeLine* Create();
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float4				m_vCamPosition = {};
	_float4x4			m_TransformMatrix[D3DTS_END] = {};
	_float4x4			m_TransformMatrix_Inverse[D3DTS_END] = {};
};

END