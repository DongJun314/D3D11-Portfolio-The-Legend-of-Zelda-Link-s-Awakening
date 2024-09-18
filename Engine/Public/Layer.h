#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* _pGameObject);
	void Priority_Tick(_float _fTimeDelta);
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta);

public:
	class CComponent* Get_Component(const wstring& _strComTag, _uint _iIndex);
	class CGameObject* Get_GameObject(_uint _iIndex);


	class CGameObject* Get_Camera();

	list<class CGameObject*> Get_List() { return m_listGameObjects; }



public:
	static CLayer* Create();
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	list<class CGameObject*> m_listGameObjects;
};

END