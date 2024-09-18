#include "Layer.h"

// Engine
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return E_FAIL;

	m_listGameObjects.push_back(_pGameObject);

	return S_OK;
}

void CLayer::Priority_Tick(_float _fTimeDelta)
{
	for (auto& pGameObject : m_listGameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Tick(_fTimeDelta);
	}
}

void CLayer::Tick(_float _fTimeDelta)
{
	for (auto& pGameObject : m_listGameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(_fTimeDelta);
	}
}

void CLayer::Late_Tick(_float _fTimeDelta)
{
	for (auto& pGameObject : m_listGameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(_fTimeDelta);
	}
}

CComponent* CLayer::Get_Component(const wstring& _strComTag, _uint _iIndex)
{
	auto iter = m_listGameObjects.begin();
	
	for (size_t i = 0; i < _iIndex; i++)
		++iter;

	return (*iter)->Get_Component(_strComTag);
}

CGameObject* CLayer::Get_GameObject(_uint _iIndex)
{
	auto	iter = m_listGameObjects.begin();

	for (size_t i = 0; i < _iIndex; i++)
		++iter;

	return *iter;
}

CGameObject* CLayer::Get_Camera()
{
	auto iter = std::next(m_listGameObjects.begin(), 0);

	CGameObject* pCamera = *iter;

	return *iter;
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_listGameObjects)
		Safe_Release(pGameObject);

	m_listGameObjects.clear();
}
