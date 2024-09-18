#pragma once

#include <process.h> // _beginthreadex¿ë Çì´õ
#include <string>
#include "GameInstance.h"
#include "Client_Anim_States.h"

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY,LEVEL_MINIGAMEA, LEVEL_MINIGAMEB, LEVEL_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	//const _uint STATE_IDLE = 0x00000001;
	//const _uint STATE_RUN = 0x00000002;
}

extern HWND				g_hWnd;
extern HINSTANCE		g_hInst;

using namespace std;
using namespace Client;