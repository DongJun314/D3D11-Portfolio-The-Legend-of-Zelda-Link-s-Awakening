#include "Renderer.h"

// Engine
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:m_pDevice(_pDevice)
	,m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP _eRenderGroup, CGameObject* _pGameObject)
{
	if (_eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_listRenderObjects[_eRenderGroup].push_back(_pGameObject);

	Safe_AddRef(_pGameObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_listRenderObjects[RENDER_PRIORITY])
	{
		if(nullptr != pGameObject)
			pGameObject->Render();

		// 위에서 Render를 호출하여 이미 그렸으니깐, 이제 필요 없기애, Release를 호출해준다
		Safe_Release(pGameObject);
	}

	m_listRenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pGameObject : m_listRenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_listRenderObjects[RENDER_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pGameObject : m_listRenderObjects[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_listRenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_listRenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_listRenderObjects[RENDER_UI].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRenderer* pInstance = new CRenderer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_listRenderObjects[i])
			Safe_Release(pGameObject);

		m_listRenderObjects[i].clear();
	}

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
