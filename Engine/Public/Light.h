#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& _LightDesc);

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	static CLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHT_DESC& _LightDesc);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	LIGHT_DESC m_LightDesc = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

};

END