#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Particle_Point;
END

BEGIN(Client)

class CParticle_Point final : public CGameObject
{
public:
	enum PARTICLE_TYPE { PT_SPREAD, PT_CHARGE, PT_END };

public:
	typedef struct EffectParticle_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		PARTICLE_TYPE eParticleType;

	}EFFECTPARTICLE_DESC;

private:
	CParticle_Point(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_Point(const CParticle_Point& _rhs);
	virtual ~CParticle_Point() = default;

public:
	void Set_ParticleType(PARTICLE_TYPE _eParticleType) {
		m_eParticleType = _eParticleType;
	}
	void Set_ParticlePos(_vector _vPos) {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Handle_ParticleType(_float _fTimeDelta);
	void Reset_Particle_LifeTime(_float _fTimeDelta);

private:
	HRESULT Add_Component();

public:
	static CParticle_Point* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;


private:
	PARTICLE_TYPE m_eParticleType = { PT_END };
	_float4x4 m_vBillBoard;
	_matrix m_Billboard_WorldMatrix;

	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Particle_Point* m_pVIBufferCom = { nullptr };
};

END