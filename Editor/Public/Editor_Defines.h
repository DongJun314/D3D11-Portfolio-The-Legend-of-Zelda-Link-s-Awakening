#pragma once

#include <math.h>
#include <string>
#include <vector>
#include <process.h>
#include <algorithm>

// ImGui
//#include "imgui_impl_dx11.h"
//#include "imgui_impl_win32.h"

// DirectXTK
#include "DirectXTK/WICTextureLoader.h"

// Engine
#include "GameInstance.h"


namespace Editor
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_NAVI, LEVEL_END };

	// For ImGui
	typedef struct{
		ID3D11ShaderResourceView* srv;
		int width;
		int height;
	} LOADTEXTURE;

}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;
using namespace Editor;