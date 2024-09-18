#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Macro.h"

namespace Engine
{
	typedef struct AnimTexture_Desc
	{
		unsigned int	iFrameNow;
		unsigned int	iFrameStart;// 시작 인덱스
		unsigned int	iFrameEnd;  // 마지막 인덱스
		DWORD			dwSpeed;
		DWORD			dwTime;
		XMUINT2			uCurRowColumn;
		XMUINT2			uRowColumn;
		XMFLOAT2		vUV;

	}ANIMTEXTURE_DESC;

	typedef struct Graphic_Desc
	{
		bool			bIsWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
		HWND			hWnd;

	} GRAPHIC_DESC;

	typedef struct Light_Desc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE			eType;
		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;

	}LIGHT_DESC;

	enum AiTextureType
	{
		AiTextureType_NONE = 0,
		AiTextureType_DIFFUSE = 1,
		AiTextureType_SPECULAR = 2,
		AiTextureType_AMBIENT = 3,
		AiTextureType_EMISSIVE = 4,
		AiTextureType_HEIGHT = 5,
		AiTextureType_NORMALS = 6,
		AiTextureType_SHININESS = 7,
		AiTextureType_OPACITY = 8,
		AiTextureType_DISPLACEMENT = 9,
		AiTextureType_LIGHTMAP = 10,
		AiTextureType_REFLECTION = 11,
		AiTextureType_BASE_COLOR = 12,
		AiTextureType_NORMAL_CAMERA = 13,
		AiTextureType_EMISSION_COLOR = 14,
		AiTextureType_METALNESS = 15,
		AiTextureType_DIFFUSE_ROUGHNESS = 16,
		AiTextureType_AMBIENT_OCCLUSION = 17,
		AiTextureType_UNKNOWN = 18,
		AiTextureType_SHEEN = 19,
		AiTextureType_CLEARCOAT = 20,
		AiTextureType_TRANSMISSION = 21,
	};


	typedef struct
	{
		class CTexture* pMtrlTextures[AiTextureType_TRANSMISSION];

	}MESH_MATERIAL_DESC;

	typedef struct ENGINE_DLL VtxPos
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[1];
	}VTXPOS;

	typedef struct ENGINE_DLL VtxPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXPOINT;

	typedef struct ENGINE_DLL VtxPosTex
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[2];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL VtxCube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[2];
	}VTXCUBE;

	typedef struct ENGINE_DLL VtxNorTex
	{
		XMFLOAT3 vPosition;
		XMFLOAT3 vNormal;
		XMFLOAT2 vTexcoord;

		static const unsigned int iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC  Elements[3];

	}VTXNORTEX;

	typedef struct ENGINE_DLL VtxMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent; //

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[4];

	}VTXMESH;

	typedef struct ENGINE_DLL VtxAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점은 어떤 뼈들(최대 4개)의 상태를 받아서 변환되어야하는가?! */
		/* iIndex = 각각의 메시가 사용하는 뼈들의 인덱스 */
		/* 메시별로 구분해서 저장할때 사용할 인덱스 */
		XMUINT4			vBlendIndices;

		/* 각각의 뼈의 상태를 몇 퍼센트나 이용할지?!(가중치 0 ~ 1) */
		/* 1 = x + y + z + w */
		XMFLOAT4		vBlendWeights;


		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[6];
	}VTXANIMMESH;

	typedef struct VtxInstance
	{
		XMFLOAT4 vRight, vUp, vLook, vTranslation, vColor;
	}VTXINSTANCE;

	typedef struct ENGINE_DLL Vtx_Rect_Instance
	{
		static const unsigned int iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC Elements[7];
	}VTX_RECT_INSTANCE;

	typedef struct ENGINE_DLL Vtx_Point_Instance
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC		Elements[7];
	}VTX_POINT_INSTANCE;
}


#endif // Engine_Struct_h__

