#pragma once

#include "GameObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CMap_GameB final : public CGameObject
{

private:
	CMap_GameB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMap_GameB(const CMap_GameB& _rhs);
	virtual ~CMap_GameB() = default;

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
	static CMap_GameB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	BoundingBox m_MeshBox;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
};

END