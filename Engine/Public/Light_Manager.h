#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLight_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& _LightDesc);

public:
	const LIGHT_DESC* Get_LightDesc(_uint _iIndex) const;

public:
	static CLight_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	list<class CLight*> m_listLights;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
};

END