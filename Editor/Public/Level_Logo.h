#pragma once

#include "Editor_Defines.h"
#include "Level.h"

BEGIN(Editor)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& _strLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END