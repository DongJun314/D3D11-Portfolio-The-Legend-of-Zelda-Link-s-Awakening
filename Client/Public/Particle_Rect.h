#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Rect;
END

BEGIN(Client)

class CParticle_Rect final : public CGameObject
{
private:
	CParticle_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_Rect(const CParticle_Rect& _rhs);
	virtual ~CParticle_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	HRESULT Set_BillBoard();

private:
	HRESULT Add_Component();

public:
	static CParticle_Rect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Rect* m_pVIBufferCom = { nullptr };
};

END
