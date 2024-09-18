#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Texture final : public CGameObject
{
public:
	enum EFFECT_TYPE { ET_HIT_BLOOD, ET_HIT_METAL, ET_CHARGING, ET_SLASH_ED, ET_ULT_LAND, ET_EXPLOSION, ET_POT_HIT, ET_END };

public:
	typedef struct EffectTexture_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		EFFECT_TYPE eEffectType;

	}EFFECTEXTURE_DESC;

private:
	CEffect_Texture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect_Texture(const CEffect_Texture& _rhs);
	virtual ~CEffect_Texture() = default;

public:
	void Set_EffectType(EFFECT_TYPE _eEffectType) {
		m_eEffectType = _eEffectType;
	}

	void Set_EffectOnce(_bool _bEffectOnce) {
		m_bEffectOnce = _bEffectOnce;
	}
	_bool Get_EffectOnce() {
		return m_bEffectOnce;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg, _uint _iLevel);
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Set_BillBoard();
	HRESULT Handle_EffectType();
	void Show_Texture();
	void Animate_Texture(_bool _bLoop);

private:
	HRESULT Add_Component();

public:
	static CEffect_Texture* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel);
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_bool m_bEffectOnce = false;
	_uint m_iTextureIndex; // 인덱스에 해당하는 텍스쳐를 사용합니다
	EFFECT_TYPE m_eEffectType = { ET_END }; 
	ANIMTEXTURE_DESC m_tAnimTex_Desc;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
};

END