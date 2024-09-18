#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
public:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float _fTimeDelta);
	HRESULT Render();

private:
	HRESULT Ready_Gara();
	HRESULT Open_Level(LEVEL _eStartLevel);
	HRESULT Ready_Interface_Prototype();

public:
	static CMainApp* Create();
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	ID3D11Device*		  m_pDevice = { nullptr };
	ID3D11DeviceContext*  m_pContext = { nullptr };
	CGameInstance*		  m_pGameInstance = { nullptr };
};

END