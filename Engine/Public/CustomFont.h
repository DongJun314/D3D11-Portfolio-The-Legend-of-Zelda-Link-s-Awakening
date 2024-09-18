#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const wstring& _strFontFilePath);
	HRESULT Render(const wstring& _strText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale);

public:
	static CCustomFont* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strFontFilePath);
	virtual void Free() override;


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	SpriteFont* m_pFont = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
};

END