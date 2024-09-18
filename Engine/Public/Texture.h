#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTexture(const CTexture& _rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& _strTextureFilePath, _uint _iNumTextures);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName, _uint _iIndex);
	HRESULT Bind_ShaderResources(class CShader* _pShader, const _char* _pConstantName);

public:
	static CTexture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strTextureFilePath, _uint _iNumTextures);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint m_iNumTextures = { 0 };
	vector<ID3D11ShaderResourceView*> m_vecSRVs;
};

END