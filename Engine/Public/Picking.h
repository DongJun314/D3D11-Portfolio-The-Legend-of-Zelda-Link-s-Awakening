#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(const GRAPHIC_DESC& _GraphicDesc);

public:
	_bool Mouse_Picking(const _matrix& _Proj, const _matrix& _ViewInverse, const _matrix& _WorldMatrix, 
		_float3* _vPickLocalPos, _float3* _vPickWorldPos, _float* _Dis,
		const class CModel* _pModel, class CGameObject** _pGameObj, _vector* _vDir, _vector* _vOrigin);

public:
	static CPicking* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const GRAPHIC_DESC& _GraphicDesc);
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float3				m_vRayLocalPos = {};
	_float3				m_vRayLocalDir = {};

	_float3				m_vRayWorldPos = {};
	_float3				m_vRayWorldDir = {};

	POINT				m_tMouse = {};
	GRAPHIC_DESC		m_GraphicDesc = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

	class CMesh* m_pMesh;

	_uint* m_pIndices = { nullptr };	// Picking을 위한 구조체
	VTXMESH* m_pVertices = { nullptr };	// Picking을 위한 구조체
};

END