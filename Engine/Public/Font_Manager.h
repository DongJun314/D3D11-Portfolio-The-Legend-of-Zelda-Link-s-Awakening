#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const wstring& _strFontTag, const wstring& _strFontFilePath);
	HRESULT Render(const wstring& _strFontTag, const wstring& _strText, const _float2& _vPosition, _fvector _vColor = Colors::White, _float fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), _float _fScale = 1.f);

private:
	class CCustomFont* Find_Font(const wstring& _strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;


private:
	map<const wstring, class CCustomFont*> m_mapFonts;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END