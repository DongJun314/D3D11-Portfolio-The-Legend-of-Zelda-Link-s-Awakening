#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CShader(const CShader& _rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	HRESULT Begin(_uint _iPassIndex, _uint _iTechniqueIndex = 0);
	HRESULT Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iLength);
	HRESULT Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV);
	HRESULT Bind_SRVs(const _char* _pConstantName, ID3D11ShaderResourceView** _ppSRV, _uint _iNumTextures);
	HRESULT Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix);
	HRESULT Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrices, _uint _iNumMatrices);

public:
	static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pElements, _uint _iNumElements);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	vector<ID3D11InputLayout*>  m_vecInputLayouts;

private:
	ID3DX11Effect*		m_pEffect = { nullptr };
};

END