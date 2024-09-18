#include "Texture.h"

// Engine
#include "Shader.h"

CTexture::CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CTexture::CTexture(const CTexture& _rhs)
	: CComponent(_rhs)
	, m_vecSRVs(_rhs.m_vecSRVs)
	, m_iNumTextures(_rhs.m_iNumTextures)
{
	for (auto& pSRV : m_vecSRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& _strTextureFilePath, _uint _iNumTextures)
{
	m_iNumTextures = _iNumTextures;

	_tchar szEXT[MAX_PATH] = TEXT("");

	_wsplitpath_s(_strTextureFilePath.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	for (size_t i = 0; i < _iNumTextures; i++)
	{
		_tchar szTextureFilePath[MAX_PATH] = TEXT("");

		wsprintf(szTextureFilePath, _strTextureFilePath.c_str(),i);
		
		HRESULT hr = { 0 };

		ID3D11ShaderResourceView* pSRV = { nullptr };

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			hr = E_FAIL;

		else
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_vecSRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, _uint _iIndex)
{
	if (_iIndex >= m_iNumTextures)
		return E_FAIL;

	return _pShader->Bind_SRV(_pConstantName, m_vecSRVs[_iIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader* _pShader, const _char* _pConstantName)
{
	return _pShader->Bind_SRVs(_pConstantName, &m_vecSRVs.front(), m_iNumTextures);
}

CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strTextureFilePath, _uint _iNumTextures)
{
	CTexture* pInstance = new CTexture(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_strTextureFilePath, _iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* _pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_vecSRVs)
		Safe_Release(pSRV);

	m_vecSRVs.clear();
}
