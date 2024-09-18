#pragma once

#include "Level.h"
#include "LandObject.h"
#include "Client_Defines.h"

BEGIN(Client)

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
	// 조명
	HRESULT Ready_Lights();

	// 카메라
	HRESULT Ready_Layer_Camera(const wstring& _strLayerTag);

	// 스카이 박스
	HRESULT Ready_Layer_SkdyBox(const wstring& _strLayerTag);
	HRESULT Ready_Layer_MiniGame(const wstring& _strLayerTag);
		
	// 맵
	HRESULT Ready_Map_Nav(const wstring& _strLayerTag);
	HRESULT Ready_Map_OBJ();
	HRESULT Ready_Map_Boss(const wstring& _strLayerTag);

	// 파티클
	HRESULT Ready_Layer_Effect(const wstring& _strLayerTag);

	// 플레이어 & 몬스터 //
	HRESULT Ready_LandObjects();
	HRESULT Ready_Layer_Player(const wstring& _strLayerTag, CLandObject::LANDOBJ_DESC& _LandObjDesc);
	HRESULT Ready_Layer_Monster(const wstring& _strLayerTag, CLandObject::LANDOBJ_DESC& _LandObjDesc);

public:
	static CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

private:
	_bool m_bPlayBGMOnce = false;
};

END