#pragma once

/* 네비게이션을 구성하는 Cell을 보관한다. */
/* 이 네비게이션컴포넌트를 이용하는 객체가 어떤 쎌 안에 있었는가?! */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct Navi_Desc
	{
		_int iStartCellIndex; //이 객체는 여기서 시작할거야~ 라는 의미
	}NAVI_DESC;

private:
	CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNavigation(const CNavigation& _rhs);
	virtual ~CNavigation() = default;

public:
	vector<class CCell*> Get_VecCell() const {
		return m_vecCells;
	}

	_int Get_CellIndex() const {
		return m_iCurrentIndex;
	}

public:
	HRESULT Initialize_Prototype(const wstring& _strNavigationDataFilePath, _float3* _pPoints = nullptr);
	HRESULT Initialize(void* _pArg) override;
	void Tick(_fmatrix _TerrainWorldMatrix);
	HRESULT Read_NavigationData(const wstring& _strNavigationDataFilePath);
	HRESULT Read_VerticeData(const _float3* _pPoints);
	_bool Is_Move(_fvector _vPosition);

	void Clear_Cell();


#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	HRESULT Make_Neighbors();

	// 네비를 깔때 만들었던 함수, 나중에 뺄 예정
	HRESULT Adjust_to_Clockwise(_float3* _pPoints);
	HRESULT Merge_Points(_float3* _pPoints); // 두 점을 하나의 점으로 합치는 함수
	HRESULT Compare_Points_Dis(const _float3 _pSourPoint, _float3* _pDestPoints); // 한 점과 여러점들의 비교

public:
	static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strNavigationDataFilePath, _float3* _pPoints = nullptr);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_int m_iCurrentIndex = { -1 }; // 현재 어떤 셀 인덱스에 위치하고 있는지를 확인하기 위한 멤버변수
	vector<class CCell*> m_vecCells; // 셀 클래스의 정점들의 정보를 관리 및 저장하기 위한 벡터 컨테이너 
	static _float4x4 m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif
};

END