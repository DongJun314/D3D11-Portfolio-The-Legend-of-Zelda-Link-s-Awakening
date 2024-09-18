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

	/* 게임내에서 사용 할 사본객체가 생성되면 자동으로 트랜스폼이 생성되게끔한다. */
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(_pArg)))
		return E_FAIL;

	// m_mapComponents 이 맵 컨테이너는 GameObject가 들고 있는 Component들의 사본(클론)을 보관하기 위한 맵 컨테이너다
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
	// 이 함수는 의미는 Component의 원형을 보관하고 있는 Component Manager에서 복제하여, 클론을 GameObject클래스에서 추후 사용하기 위해서 맵 컨테이너에 보관하는 함수이다 
	auto	iter = m_mapComponents.find(_strComTag); // 여기서는 중복된 사본을 가지고 있는지 체크, 없어야지 밑으로 간다 
	if (iter != m_mapComponents.end())
		return E_FAIL;

	CComponent* pComponent = m_pGameInstance->Clone_Component(_iLevelIndex, _strPrototypeTag, _pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponents.emplace(_strComTag, pComponent);

	// 이 줄의 의미는 Add Component 이 함수를 호출한 클래스에서 해당 컴포넌트를 멤버변수로 소유하기 위해서
	*_ppOut = pComponent;

	// 포인터 주소를 대입이 일어났기에, 레퍼카운트를 증가 시켜준다
	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent* CGameObject::Get_Component(const wstring& _strComTag)
{
	// 이 함수는 매개변수로 전달 받은 키값으로 저장되어 있는 Component가 있을 경우 그 Component의 포인터를 반환하는 함수이다
	auto iter = m_mapComponents.find(_strComTag);

	if (iter == m_mapComponents.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Clear_Component(const wstring& _strComTag)
{
	auto	iter = m_mapComponents.find(_strComTag); // 여기서는 중복된 사본을 가지고 있는지 체크, 없어야지 밑으로 간다 
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
