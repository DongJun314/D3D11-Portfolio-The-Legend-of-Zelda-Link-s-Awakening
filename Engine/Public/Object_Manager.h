#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint _iNumLevels);
	void Priority_Tick(_float _fTimeDelta);
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta);

public:
	class CComponent* Get_Component(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex);
	class CGameObject* Get_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, _uint _iIndex);

public:
	HRESULT Add_Prototype(const wstring& _strPrototypeTag, class CGameObject* _pPrototype);
	HRESULT Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg);
	class CLayer* Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag);
	class CGameObject* Clone_GameObject(const wstring& _strPrototypeTag, void* _pArg, _uint _iLevel);
	class CGameObject* Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag);
	void Clear(_uint _iLevelIndex);
	void Clear_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag);

private:
	class CGameObject* Find_Prototype(const wstring& _strPrototypeTag);

public:
	static CObject_Manager* Create(_uint _iNumLevels);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint m_iNumLevels = { 0 };

private:
	map<const wstring, class CGameObject*>	m_mapPrototypes; // 원형을 보관하는 맵
	map<const wstring, class CLayer*>*      m_pMapLayers = { nullptr }; // 사본을 보관하는 맵
	typedef map<const wstring, class CLayer*> LAYERS;
};

END