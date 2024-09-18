#pragma once

#include "Base.h"
#include "Editor_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Editor)

class CEditorApp final : public CBase
{
private:
	CEditorApp();
	virtual ~CEditorApp() = default;

public:
	HRESULT Initialize();
	void	Tick(_float _fTimeDelta);
	HRESULT Render();

private:
	HRESULT Open_Level(LEVEL _eStartLevel);
	HRESULT Ready_Prototype_Component_Static();

public:
	static CEditorApp* Create();
	virtual void Free() override;

	///////////////////////////////////////////////////
	////////////////* Member Variable *////////////////
	///////////////////////////////////////////////////
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	class CGameInstance* m_pGameInstance = { nullptr };
	class CWindow* m_pWindow = { nullptr };
	class CImGui_Manager* m_pImGui_Manager = { nullptr };
};

END