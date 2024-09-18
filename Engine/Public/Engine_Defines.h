#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXTK/Effects.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>

#include <Effects11/d3dx11effect.h>

// assimp 관련된 헤더파일
#include <assimp\scene.h> // importer로 뜯은 파일을 저장하기 위한 구조체 헤더
#include <assimp\Importer.hpp> // 파일을 뜯어서 읽기 위한 헤더
#include <assimp\postprocess.h> // model를 로드할때, Flag를 추가하기 위한 헤더

// fmod 헤더 파일
#include <Fmod/fmod.h>
#include <Fmod/fmod.hpp>
//#pragma comment(lib, "fmodex_vc.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fmodL_vc.lib")

using namespace DirectX;

#include <map>
#include <list>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
using namespace std;

namespace Engine
{
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };
	enum KEYINPUTSTATE { KI_Z, KI_X,KI_SPACE, KI_UP, KI_DOWN, KI_LEFT, KI_RIGHT, DIKI_END};
	enum class SOUNDTYPE {
		BGM,		// bgm 사운드

		// 플레이어
		SLASH,
		SHIELD,
		HIT,
		CHARGE,
		PARTICLE,
		SWING,
		WALK,
		ULT,
		LANDING,
		JUMP,

		// 몬스터
		MON_GUARD,
		MON_DAMAGE,
		MON_WALK,
		MON_DEAD,
		MON_ATK,
		MON_PUSH,
		MON_APPEAR,

		ENUM_END	// 최대 채널
	};

	const wstring g_strTransformTag = TEXT("Com_Transform");
}

#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
//#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG