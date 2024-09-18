#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CNavMesh final : public CGameObject
{
private:
	CNavMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CNavMesh(const CNavMesh& _rhs);
	virtual ~CNavMesh() = default;

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
	static CNavMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	CNavigation* m_pNavigationCom = { nullptr };
	CNavigation* m_pGameA_NavigationCom = { nullptr };
	CNavigation* m_pBossMap_NavigationCom = { nullptr };
};

END