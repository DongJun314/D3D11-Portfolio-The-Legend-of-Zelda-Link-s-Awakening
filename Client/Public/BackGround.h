#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	typedef struct BackGound_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iBackData = { 0 };
	}BACKGROUND_DESC;

private:
	CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBackGround(const CBackGround& _rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg, _uint _iLevel);
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component(_uint _iLevel);

public:
	static CBackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_uint m_iBackData = { 0 };
	_float m_fX = { 0.f };
	_float m_fY = { 0.f }; 
	_float m_fSizeX = { 0.f };
	_float m_fSizeY = { 0.f };
	_float4x4 m_ViewMatrix = {};
	_float4x4 m_ProjMatrix = {};

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pLogoTextureCom = { nullptr };
	CTexture* m_pLoadingTextureCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
};

END