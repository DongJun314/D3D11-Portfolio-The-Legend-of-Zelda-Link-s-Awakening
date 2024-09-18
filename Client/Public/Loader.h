#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL _eNextLevel);
	HRESULT Loading_Selector();
	_bool isFinished() const { return m_bIsFinished; }

private:
	HRESULT Loading_For_GamePlayLevel();
	HRESULT Loading_For_MiniGameA();
	HRESULT Loading_For_MiniGameB();

public:
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL _eNextLevel);
	virtual void Free() override;

#ifdef _DEBUG
public:
	void Output_LoadingText();
#endif 

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	_bool					m_bIsFinished = false;
	wstring					m_strLoadingText = {};

	LEVEL					m_eNextLevel = { LEVEL_END };
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_CriticalSection = {};
	 
private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	class CGameInstance*			m_pGameInstance = { nullptr };
};

END