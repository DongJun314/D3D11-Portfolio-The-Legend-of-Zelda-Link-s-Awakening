#pragma once

#include "GameObject.h"

// Editor
#include "Editor_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Editor)

class CPlayer final : public CGameObject
{

private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& _rhs);
	virtual ~CPlayer() = default;

public:
	_float3 GetPickLocalPos() const {
		return m_vPickLocalPos;
	}

	_float3 GetPickWorldPos() const {
		return m_vPickWorldPos;
	}

	_bool IsPicked() const {
		return m_bIsPicked;
	}

	void SetIsPicked(_bool _bIsPicked) { m_bIsPicked = _bIsPicked; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Tick(_float _fTimeDelta) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_float3 m_vPickLocalPos = _float3(0.f, 0.f, 0.f);
	_float3 m_vPickWorldPos = _float3(0.f, 0.f, 0.f);

	_bool m_bIsPicked = false;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	BoundingBox m_MeshBox;
};

END
