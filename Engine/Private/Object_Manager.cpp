#include "Object_Manager.h"

// Engine
#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint _iNumLevels)
{
    if (nullptr != m_pMapLayers)
        return E_FAIL;

    m_pMapLayers = new LAYERS[_iNumLevels];

    m_iNumLevels = _iNumLevels;

    return S_OK;
}

void CObject_Manager::Priority_Tick(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pMapLayers[i])
            Pair.second->Priority_Tick(_fTimeDelta);
    }
}

void CObject_Manager::Tick(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pMapLayers[i])
            Pair.second->Tick(_fTimeDelta);
    }
}

void CObject_Manager::Late_Tick(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pMapLayers[i])
            Pair.second->Late_Tick(_fTimeDelta);
    }
}

CComponent* CObject_Manager::Get_Component(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strComTag, _uint _iIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Get_Component(_strComTag, _iIndex);
}

CGameObject* CObject_Manager::Get_GameObject(_uint _iLevelIndex, const wstring& _strLayerTag, _uint _iIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Get_GameObject(_iIndex);
}

HRESULT CObject_Manager::Add_Prototype(const wstring& _strPrototypeTag, CGameObject* _pPrototype)
{
    // �� �Լ��� ���� ��ü�� Ű ���� �Բ� �ش� ���� ��ü�� �����͸� �� �����̳��� �����ϴ� �Լ��̴� 
    if (nullptr != Find_Prototype(_strPrototypeTag))
        return E_FAIL; 

    m_mapPrototypes.emplace(_strPrototypeTag, _pPrototype);

    return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag, const wstring& _strPrototypeTag, void* _pArg)
{
    /*�� �Լ��� ���� ��ü�� �� �����̳ʿ� �������� ����� Ű������, ���� ��ü�� ã��, �� ���� ��ü�� �����ͷ� Clone�Լ��� ȣ���Ͽ�, Ŭ���� �����,
    �� Ŭ�е��� Ư�� ���̾ ����־� �����Ѵ� */ 

     CGameObject* pPrototype = Find_Prototype(_strPrototypeTag);
    if (nullptr == pPrototype)
        return E_FAIL;

    CGameObject* pGameObject = pPrototype->Clone(_pArg, _iLevelIndex);
    if (nullptr == pGameObject)
        return E_FAIL;

    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);


    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();
        if (nullptr == pLayer)
            return E_FAIL;

        pLayer->Add_GameObject(pGameObject);

        m_pMapLayers[_iLevelIndex].emplace(_strLayerTag, pLayer);
    }
    else
    {
        pLayer->Add_GameObject(pGameObject);
    }

    return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject(const wstring& _strPrototypeTag, void* _pArg, _uint _iLevel)
{
    CGameObject* pPrototype = Find_Prototype(_strPrototypeTag);
    if (nullptr == pPrototype)
        return nullptr;

    return pPrototype->Clone(_pArg, _iLevel);
}

CGameObject* CObject_Manager::Find_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    if (_iLevelIndex >= m_iNumLevels)
        return nullptr;


    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

    auto iter = m_pMapLayers[_iLevelIndex].find(_strLayerTag);
    if (iter == m_pMapLayers[_iLevelIndex].end())
        return nullptr;

    return iter->second->Get_List().back();
}

void CObject_Manager::Clear(_uint _iLevelIndex)
{
    for (auto& Pair : m_pMapLayers[_iLevelIndex])
        Safe_Release(Pair.second);

    m_pMapLayers[_iLevelIndex].clear();
}

void CObject_Manager::Clear_CloneObject(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (nullptr == pLayer)
        return;

    m_pMapLayers[_iLevelIndex].erase(_strLayerTag);
}

CGameObject* CObject_Manager::Find_Prototype(const wstring& _strPrototypeTag)
{
    auto iter = m_mapPrototypes.find(_strPrototypeTag);

    if (iter == m_mapPrototypes.end())
        return nullptr;

    return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint _iLevelIndex, const wstring& _strLayerTag)
{
    if (_iLevelIndex >= m_iNumLevels)
        return nullptr;

    auto iter = m_pMapLayers[_iLevelIndex].find(_strLayerTag);
    if (iter == m_pMapLayers[_iLevelIndex].end())
        return nullptr;

    return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint _iNumLevels)
{
    CObject_Manager* pInstance = new CObject_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels)))
    {
        MSG_BOX("Failed to Created : CObject_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Manager::Free()
{
    for (size_t i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_pMapLayers[i])
            Safe_Release(Pair.second);
        m_pMapLayers[i].clear();
    }

    Safe_Delete_Array(m_pMapLayers);

    for (auto& Pair : m_mapPrototypes)
        Safe_Release(Pair.second);
    m_mapPrototypes.clear();
}
