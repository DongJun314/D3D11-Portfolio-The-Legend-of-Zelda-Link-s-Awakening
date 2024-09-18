#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CNavMesh_MiniGameA final : public CGameObject
{
private:
	CNavMesh_MiniGameA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNavMesh_MiniGameA(const CNavMesh_MiniGameA& _rhs);
	virtual ~CNavMesh_MiniGameA() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();

public:
	static CNavMesh_MiniGameA* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	CNavigation* m_pNavigationCom = { nullptr };
};

END