#include "GameObject.h"

// Engine
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& _rhs)
	:m_pDevice(_rhs.m_pDevice)
	,m_pContext(_rhs.m_pContext)
	,m_pGameInstance(_rhs.m_pGameInstance)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* _pArg)
{
	if (nullptr != _pArg)
	{
		GAMEOBJECT_DESC* pGameDesc = (GAMEOBJECT_DESC*)_pArg;
		{
			m_iGameObjectData = pGameDesc->iGameObjectData;
		}
	}

	/* ���ӳ����� ��� �� �纻��ü�� �����Ǹ� �ڵ����� Ʈ�������� �����ǰԲ��Ѵ�. */
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(_pArg)))
		return E_FAIL;

	// m_mapComponents �� �� �����̳ʴ� GameObject�� ��� �ִ� Component���� �纻(Ŭ��)�� �����ϱ� ���� �� �����̳ʴ�
	m_mapComponents.emplace(g_strTransformTag, m_pTransformCom);
	Safe_AddRef(m_pTransformCom);

	return S_OK;
}

void CGameObject::Priority_Tick(_float _fTimeDelta)
{
}

void CGameObject::Tick(_float _fTimeDelta)
{
}

void CGameObject::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint _iLevelIndex, const wstring& _strPrototypeTag, const wstring& _strComTag, CComponent** _ppOut, void* _pArg)
{
	// �� �Լ��� �ǹ̴� Component�� ������ �����ϰ� �ִ� Component Manager���� �����Ͽ�, Ŭ���� GameObjectŬ�������� ���� ����ϱ� ���ؼ� �� �����̳ʿ� �����ϴ� �Լ��̴� 
	auto	iter = m_mapComponents.find(_strComTag); // ���⼭�� �ߺ��� �纻�� ������ �ִ��� üũ, ������� ������ ���� 
	if (iter != m_mapComponents.end())
		return E_FAIL;

	CComponent* pComponent = m_pGameInstance->Clone_Component(_iLevelIndex, _strPrototypeTag, _pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponents.emplace(_strComTag, pComponent);

	// �� ���� �ǹ̴� Add Component �� �Լ��� ȣ���� Ŭ�������� �ش� ������Ʈ�� ��������� �����ϱ� ���ؼ�
	*_ppOut = pComponent;

	// ������ �ּҸ� ������ �Ͼ�⿡, ����ī��Ʈ�� ���� �����ش�
	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent* CGameObject::Get_Component(const wstring& _strComTag)
{
	// �� �Լ��� �Ű������� ���� ���� Ű������ ����Ǿ� �ִ� Component�� ���� ��� �� Component�� �����͸� ��ȯ�ϴ� �Լ��̴�
	auto iter = m_mapComponents.find(_strComTag);

	if (iter == m_mapComponents.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Clear_Component(const wstring& _strComTag)
{
	auto	iter = m_mapComponents.find(_strComTag); // ���⼭�� �ߺ��� �纻�� ������ �ִ��� üũ, ������� ������ ���� 
	if (iter != m_mapComponents.end())
	{
		m_mapComponents.erase(iter);
	}
	
	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_mapComponents)
		Safe_Release(Pair.second);
	m_mapComponents.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
