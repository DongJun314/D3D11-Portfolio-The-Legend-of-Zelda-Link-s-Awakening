#pragma once

#include "Window.h"
#include "Editor_Defines.h"

BEGIN(Editor)

class CLeft_Window final : public CWindow
{
private:
	CLeft_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLeft_Window() = default;

public:
	virtual HRESULT Initialize();
	virtual void	Tick(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	static CLeft_Window* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
	bool m_bMode;
};

END