#include "Font_Manager.h"

// Engine
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Initialize()
{
    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const wstring& _strFontTag, const wstring& _strFontFilePath)
{
    if (nullptr != Find_Font(_strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, _strFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_mapFonts.emplace(_strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render(const wstring& _strFontTag, const wstring& _strText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, _float _fScale)
{
    CCustomFont* pFont = Find_Font(_strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(_strText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

CCustomFont* CFont_Manager::Find_Font(const wstring& _strFontTag)
{
    auto	iter = m_mapFonts.find(_strFontTag);

    if (iter == m_mapFonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont_Manager::Free()
{
    for (auto& Pair : m_mapFonts)
        Safe_Release(Pair.second);
    m_mapFonts.clear();

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
