#pragma once

#include "GameObject.h"

// Client
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CMap_GameA final : public CGameObject
{
public:
	enum STAGE { STAGE_1, STAGE_2, STAGE_3, STAGE_4, STAGE_END };

private:
	CMap_GameA(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMap_GameA(const CMap_GameA& _rhs);
	virtual ~CMap_GameA() = default;

public:
	_uint Get_Stage_HitCount(STAGE _eStage);
	_bool Get_StageDataOnce(STAGE _eStage);
	HRESULT Add_HitCount(STAGE _eStage);

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
	static CMap_GameA* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg, _uint _iLevel) override;
	virtual void Free() override;

	//---------------------------------------------------------------------------------------
	/**Member Variable
	//---------------------------------------------------------------------------------------*/
private:
	_uint m_iStage1_HitCount = { 0 };
	_uint m_iStage2_HitCount = { 0 };
	_uint m_iStage3_HitCount = { 0 };
	_uint m_iStage4_HitCount = { 0 };

	_bool m_bStage1DataOnce = false;
	_bool m_bStage2DataOnce = false;
	_bool m_bStage3DataOnce = false;
	_bool m_bStage4DataOnce = false;
	_bool m_bLockOnce = false;
	BoundingBox m_MeshBox;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
};

END