#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void	Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	ID3D11Device*			 m_pDevice = { nullptr };
	ID3D11DeviceContext*	 m_pContext = { nullptr };

	class CGameInstance*	 m_pGameInstance = { nullptr };
};

END