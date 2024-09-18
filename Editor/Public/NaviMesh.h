#pragma once

#include "GameObject.h"

// Editor
#include "Editor_Defines.h"

BEGIN(Editor)

class CNaviMesh final : public CGameObject
{
public:
	typedef struct CellPoints
	{
		XMFLOAT3 A;
		XMFLOAT3 B;
		XMFLOAT3 C;
	}CELLS;


private:
	CNaviMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNaviMesh(const CNaviMesh& _rhs);
	virtual ~CNaviMesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Adjust_to_Clockwise(_float3* _pPoints);
	HRESULT Merge_Points(CELLS* _tCells); // 두 점을 하나의 점으로 합치는 함수
	HRESULT Compare_Points_Dis(const _float3& _pSourPoint, CELLS* _tCells); // 한 점과 여러점들의 비교

public:
	static CNaviMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	BoundingBox m_MeshBox;

private:
	// NaviMesh
	_uint m_iCount = { 0 };
	_float3 m_vPickLocalPos = _float3(0.f, 0.f, 0.f);
	_float3 m_vPickWorldPos = _float3(0.f, 0.f, 0.f);
	vector<_float3> m_vecPoints;
	vector<class CNavigation*> m_vecNaviCom;
	wstring sizeWStr;

	vector<CELLS> m_vecCells;
};

END
