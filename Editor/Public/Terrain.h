#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
END

BEGIN(Editor)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	CTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTerrain(const CTerrain& _rhs);
	virtual ~CTerrain() = default;

public:
	_float3 GetPickLocalPos() const {
		return m_vPickLocalPos;
	}

	_float3 GetPickWorldPos() const {
		return m_vPickWorldPos;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


	///////////////////////////////////////////////////
	////////////////* Member Variable *////////////////
	///////////////////////////////////////////////////
private:
	_float3 m_vPickLocalPos;
	_float3 m_vPickWorldPos;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };
};

END