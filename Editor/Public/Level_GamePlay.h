#pragma once

#include "Editor_Defines.h"
#include "Level.h"

BEGIN(Editor)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& _strLayerTag);
	HRESULT Ready_Map_OBJ(wstring _wstrFileName);
	HRESULT Ready_Layer_ForkLift(const wstring& _strLayerTag); // Terrain
	HRESULT Ready_Layer_Terrain(const wstring& _strLayerTag); // Terrain
	HRESULT Ready_Layer_Mesh_Terrain(const wstring& _strLayerTag); // Marin House Model

public:
	static CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;


private:
	_bool m_bIsCloned = false;

private:
	class CImGui_Manager* m_pImGui_Manager = { nullptr };
};

END