#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;


public:
	/* 그래픽 디바이스의 초기화. */
	HRESULT Ready_Graphic_Device(_In_ const GRAPHIC_DESC& _GraphicDesc, _Inout_ ID3D11Device** _ppDevice,
		_Inout_ ID3D11DeviceContext** _ppContext);

	/* 백버퍼를 지운다. */
	HRESULT Clear_BackBuffer_View(_float4 _vClearColor);

	/* 깊이버퍼 + 스텐실버퍼를 지운다. */
	HRESULT Clear_DepthStencil_View();

	/* 후면 버퍼를 전면버퍼로 교체한다.(백버퍼를 화면에 직접 보여준다.) */
	HRESULT Present();

private:
	/* 스왑체인을 만들었다 == 백버퍼(텍스쳐)가 생성된다. */
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
	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드..Create..) 컴객체의 생성과 관련된 역할 */
	ID3D11Device* m_pDevice = { nullptr };

	/* 기능실행.(바인딩작업, 정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	/* 후면버퍼와 전면버퍼를 교체해가면서 화면에 보여주는 역할 */
	IDXGISwapChain* m_pSwapChain = { nullptr };

	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };
};

END