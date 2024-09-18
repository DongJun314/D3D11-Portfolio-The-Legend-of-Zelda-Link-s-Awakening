#pragma once

/* �׺���̼��� �����ϴ� Cell�� �����Ѵ�. */
/* �� �׺���̼�������Ʈ�� �̿��ϴ� ��ü�� � �� �ȿ� �־��°�?! */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct Navi_Desc
	{
		_int iStartCellIndex; //�� ��ü�� ���⼭ �����Ұž�~ ��� �ǹ�
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

	// �׺� �� ������� �Լ�, ���߿� �� ����
	HRESULT Adjust_to_Clockwise(_float3* _pPoints);
	HRESULT Merge_Points(_float3* _pPoints); // �� ���� �ϳ��� ������ ��ġ�� �Լ�
	HRESULT Compare_Points_Dis(const _float3 _pSourPoint, _float3* _pDestPoints); // �� ���� ���������� ��

public:
	static CNavigation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strNavigationDataFilePath, _float3* _pPoints = nullptr);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_int m_iCurrentIndex = { -1 }; // ���� � �� �ε����� ��ġ�ϰ� �ִ����� Ȯ���ϱ� ���� �������
	vector<class CCell*> m_vecCells; // �� Ŭ������ �������� ������ ���� �� �����ϱ� ���� ���� �����̳� 
	static _float4x4 m_WorldMatrix;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif
};

END