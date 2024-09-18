#pragma once

#include "PotDemonPartObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CRasengan final : public CPotDemonPartObject
{
public:
	typedef struct Rasengan_Desc : public CPotDemonPartObject::POTDEMON_PARTOBJ_DESC
	{
		_int* iRasenganState;
		_uint iRasengan_CloneCount;

		// 모델의 뼈들 중에서 특히나 다른 모델의 붙여야하는 뼈들 있다, 그걸 보통 주로 Socket Bone이라고 한다
		_float4x4* pSocketMatrix;

	}RASENGAN_DESC;

private:
	CRasengan(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRasengan(const CRasengan& _rhs);
	virtual ~CRasengan() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Collision_ToPlayer();
	_bool Collision_ToPlayerBody();
	_bool Collision_ToPlayerSword();
	_bool Collision_ToPlayerShield();
	void Invalidate_TransformationMatrix();

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();
	void Handle_Spheres_Sliding(CGameObject* _pGameObject);

public:
	static CRasengan* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_vector m_vMonsterPos;
	_vector m_vDirToPlayer;
	_vector m_vDirToMonster;

	_bool* m_bOnFireRasengan;
	_int* m_iRasenganState;
	_uint m_iRasengan_CloneCount;

	_float m_fSetScaling = { 0.f };

	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	_float4x4 m_RasenganWorldMatrix;
	_float4x4* m_pSocketMatrix = { nullptr };
	_float4x4* m_pTransformationMatrix = { nullptr };
};

END