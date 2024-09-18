#include "Shader.h"

CShader::CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{

}

CShader::CShader(const CShader& _rhs)
	: CComponent(_rhs)
	, m_pEffect(_rhs.m_pEffect)
	, m_vecInputLayouts(_rhs.m_vecInputLayouts)
{
	for (auto& pInputLayout : m_vecInputLayouts)
		Safe_AddRef(pInputLayout);

	Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements)
{
	_uint iHlsIFlag = { 0 };

#ifdef _DEBUG
	iHlsIFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlsIFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG
	
	///////////////////////////////
	/* 셰이더를 컴 객체화 시키기 */
	///////////////////////////////
	if (FAILED(D3DX11CompileEffectFromFile(_strShaderFilePath.c_str(),
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iHlsIFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	//////////////////////////////////////////////////////////
	/* Shader파일 안에 Pass의 갯수의 따라 InputLayout 만들기*/
	//////////////////////////////////////////////////////////
	D3DX11_EFFECT_DESC EffectDesc = {};
	m_pEffect->GetDesc(&EffectDesc);

	for (uint32_t i = 0; i < EffectDesc.Techniques; i++)
	{
		ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(i);
		if (nullptr == pTechnique)
			return E_FAIL;

		D3DX11_TECHNIQUE_DESC TechniqueDesc = {};
		pTechnique->GetDesc(&TechniqueDesc);

		for (uint32_t j = 0; j < TechniqueDesc.Passes; j++)
		{
			ID3D11InputLayout* pInputLayout = { nullptr };

			ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(j);
			if (nullptr == pPass)
				return E_FAIL;

			D3DX11_PASS_DESC PassDesc = {};
			pPass->GetDesc(&PassDesc);

			if (FAILED(m_pDevice->CreateInputLayout(_pElements, _iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
				return E_FAIL;

			m_vecInputLayouts.push_back(pInputLayout);
		}

	}

	

	return S_OK;
}

HRESULT CShader::Initialize(void* _pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint _iPassIndex, _uint _iTechniqueIndex)
{
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(_iTechniqueIndex);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(_iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_vecInputLayouts[_iPassIndex]);

	return S_OK;
}

HRESULT CShader::Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(_pData, 0, _iLength);
}

HRESULT CShader::Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV)
{
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

 	return pSRVariable->SetResource(_pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* _pConstantName, ID3D11ShaderResourceView** _ppSRV, _uint _iNumTextures)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(_ppSRV, 0, _iNumTextures);
}

HRESULT CShader::Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)_pMatrix);
}

HRESULT CShader::Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)_pMatrices, 0, _iNumMatrices);
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements)
{
	CShader* pInstance = new CShader(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_strShaderFilePath, _pElements, _iNumElements)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* _pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_vecInputLayouts)
		Safe_Release(pInputLayout);

	Safe_Release(m_pEffect);
}
