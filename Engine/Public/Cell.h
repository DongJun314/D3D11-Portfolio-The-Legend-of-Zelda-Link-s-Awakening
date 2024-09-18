#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	// 가독성이 좋도록 열거체로 구분해준다
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END }; 
	enum LINES { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCell() = default;

public:
	class CVIBuffer_Cell* Get_VIBuffer_Cell() const {
		return m_pVIBuffer;
	}
	_float3 Get_Point(POINTS _ePoint) {
		return m_vPoints[_ePoint];
	}

	void Set_Neighbor(LINES _eLine, const CCell* _pNeighbor) {
		m_iNeighborIndices[_eLine] = _pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* _pPoints, _uint _iCellIndex);
	_bool Compare_Points(const _float3& _vSourPoint, const _float3& _vDestPoint);
	_bool Is_In(_fvector _vLocalPos, _int* _pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

public:
	static CCell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, _uint _iCellIndex);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float3 m_vPoints[POINT_END]; // 삼각형을 구성하는 점 3개를 받아오도록 해준다
	_uint m_iIndex = { 0 };

	_float3 m_vNormals[LINE_END];
	_int m_iNeighborIndices[LINE_END] = { -1, -1, -1 };


private:
	ID3D11Device* m_pDevice = { nullptr }; // 어떤 셀에 있는지 그리기 위한 Device, Context
	ID3D11DeviceContext* m_pContext = { nullptr }; // 어떤 셀에 있는지 그리기 위한 Device, Context

#ifdef _DEBUG
private:
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif
};

END