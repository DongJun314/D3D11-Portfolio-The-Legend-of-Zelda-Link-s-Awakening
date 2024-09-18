#pragma once

#include "Base.h"
#include "Editor_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Editor)

class CWindow : public CBase
{
protected:
	CWindow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CWindow() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	static CWindow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;


//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };
	class CLeft_Window* m_pLeftWindow = { nullptr };
	class CRight_Window* m_pRightWindow = { nullptr };
};

END