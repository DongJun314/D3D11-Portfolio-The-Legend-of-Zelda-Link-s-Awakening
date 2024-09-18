#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GameObject_Desc : public CTransform::TRANSFORM_DESC
	{
		_uint iGameObjectData = { 0 };
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject(const CGameObject& _rhs);
	virtual ~CGameObject() = default;

public:
	void Set_KeyValue(wstring _wstrKeyValue) { m_wstrKeyForImGui = _wstrKeyValue; }
	const wstring& Get_KeyValue() { return m_wstrKeyForImGui; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void Priority_Tick(_float _fTimeDelta);
	virtual void Tick(_float _fTimeDelta);
	virtual void Late_Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Get_Component(const wstring& _strComTag);
	HRESULT Clear_Component(const wstring& _strComTag);

public:
	HRESULT Add_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, const wstring& _strComTag, class CComponent** _ppOut, void* _pArg = nullptr);

public:
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) = 0;
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	_uint m_iLevel = { 0 };
	_uint m_iGameObjectData = { 0 };
	wstring m_wstrKeyForImGui = TEXT("Nothing");
	map<const wstring, class CComponent*> m_mapComponents;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*	m_pGameInstance = { nullptr };
	class CTransform*		m_pTransformCom = { nullptr };
};

END