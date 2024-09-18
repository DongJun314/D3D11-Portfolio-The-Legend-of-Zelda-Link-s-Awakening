#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;


public:
	/* �׷��� ����̽��� �ʱ�ȭ. */
	HRESULT Ready_Graphic_Device(_In_ const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice,
		_Inout_ ID3D11DeviceContext** _ppContext);

	/* ����۸� �����. */
	HRESULT Clear_BackBuffer_View(_float4 _vClearColor);

	/* ���̹��� + ���ٽǹ��۸� �����. */
	HRESULT Clear_DepthStencil_View();

	/* �ĸ� ���۸� ������۷� ��ü�Ѵ�.(����۸� ȭ�鿡 ���� �����ش�.) */
	HRESULT Present();

private:
	/* ����ü���� ������� == �����(�ؽ���)�� �����ȴ�. */
	HRESULT Ready_SwapChain(HWND _hWnd, _bool _bIsWindowed, _uint _iWinCX, _uint _iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint _iWinCX, _uint _iWinCY);

public:
	static CGraphic_Device* Create(_In_ const GRAPHIC_DESC& _GraphicDesc, _Out_ ID3D11Device** _ppDevice,
		_Out_ ID3D11DeviceContext** _ppContext);
	virtual void Free() override;

//---------------------------------------------------------------------------------------
/**Member Variable
//---------------------------------------------------------------------------------------*/
private:
	/* �޸� �Ҵ�. (��������, �ε�������, �ؽ��ķε�..Create..) �İ�ü�� ������ ���õ� ���� */
	ID3D11Device* m_pDevice = { nullptr };

	/* ��ɽ���.(���ε��۾�, �������۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	/* �ĸ���ۿ� ������۸� ��ü�ذ��鼭 ȭ�鿡 �����ִ� ���� */
	IDXGISwapChain* m_pSwapChain = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };
};

END