#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL _eNextLevel);
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_BackGround(const wstring& _strLayerTag);
	HRESULT Ready_Layer_UI();

public:
	static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL _eNextLevel);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	LEVEL			m_eNextLevel = { LEVEL_END };

private:
	class CLoader*  m_pLoader = { nullptr };
};

END