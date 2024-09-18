#include "stdafx.h"
#include "Loader.h"

// Client
#include "Sky.h"
#include "Weapon.h"
#include "Player.h"
#include "NavMesh.h"
#include "Terrain.h"
#include "Collider.h"
#include "MapObject.h"
#include "Map_GameA.h"
#include "Map_GameB.h"
#include "BackGround.h"

// MiniGameA
#include "FireBall.h"
#include "Mini_GameA.h"
#include "Mini_GameA_Back.h"
#include "Albatoss.h"
#include "FlyingOctorok.h"
#include "BonePutter.h"
#include "Piccolo.h"

#include "Mini_GameB.h"
#include "Body_Player.h"
#include "Camera_Free.h"
#include "Mesh_Terrain.h"
#include "Particle_Rect.h"
#include "Particle_Point.h"

// Monster
#include "Zombie.h"
#include "BlueMoriblin.h"
#include "StalfosOrange.h"

// Effect
#include "Effect_Texture.h"


// Boss Monster
#include "Cucco.h"
#include "PotDemon.h"
#include "Rasengan.h"
#include "Boss_Map.h"
#include "BomberGreen.h"
#include "PotDemonPot.h"
#include "PotDemonKing.h"
#include "ChainIronBall.h"
#include "Collider_MainToBoss.h"
#include "Collider_BossToMain.h"

// Engine
#include "GameInstance.h"

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

_uint APIENTRY wThreadMain(void* _pArg)
{
	// 이건 무엇일까? 
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(_pArg);

	pLoader->Loading_Selector();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL _eNextLevel)
{
	m_eNextLevel = _eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, wThreadMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_Selector()
{
	EnterCriticalSection(&m_CriticalSection);
	
	HRESULT hr = {};

	switch (m_eNextLevel)
	{
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	// 추후 추가 적인 로딩화면 추가
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

#ifdef _DEBUG
void CLoader::Output_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());
}
#endif // DEBUG

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	///////////////
	// Component //
	///////////////
	{	
		// VIBuffer 버퍼 //
		{
			m_strLoadingText = TEXT("버퍼를(을) 로딩 중 입니다.");
			/* For.Prototype_Component_VIBuffer_Rect */
 			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
				CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For.Prototype_Component_VIBuffer_Cube */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
				CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For.Prototype_Component_VIBuffer_Particle_Rect */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Rect"),
				CVIBuffer_Particle_Rect::Create(m_pDevice, m_pContext, 300))))
				return E_FAIL;

			/* For.Prototype_Component_VIBuffer_Particle_Point */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Point"),
				CVIBuffer_Particle_Point::Create(m_pDevice, m_pContext, 2000))))
				return E_FAIL;
		}

		// Texture 텍스쳐 //
		{
			/* For.Prototype_Component_Texture_Sky */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 9))))
				return E_FAIL;

			/* For.Prototype_Component_Texture_Snow */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/snow.png"), 1))))
				return E_FAIL;

			///* For.Prototype_Component_Texture_Pixel_Effect */
			//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Pixel_Effect"),
			//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Pixel/Pixel_Effect_%d.png"), 20))))
			//	return E_FAIL;

			/* For.Prototype_Component_Texture_VFX_Effect */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_VFX_Effect"),
				CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/VFX/VFX_%d.png"), 18))))
				return E_FAIL;

			///* For.Prototype_Component_Texture_FA01_Effect */
			//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FA01_Effect"),
			//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/FA01/FA01_%d.png"), 10))))
			//	return E_FAIL;


			///* For.Prototype_Component_Texture_Snow */
			//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
			//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/snow.png"), 1))))
			//	return E_FAIL;
		}

		// Model 모델 로딩 //
		{
			m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
			/* For.Prototype_Component_Model_Link */
			_matrix			PivotMatrix = XMMatrixIdentity();
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Link"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/Link_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_BlueMoriblinSword */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BlueMoriblinSword"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/BlueMoriblinSword_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_StalfosOrange*/
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_StalfosOrange"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/StalfosOrange_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_BlueMoblinSword */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zombie"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/Zombie_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_PotDemon_Pot */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PotDemon_Pot"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/PotDemonPot_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_PotDemon_King */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PotDemon_King"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/PotDemonKing_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_BomberGreen */
			PivotMatrix = XMMatrixMultiply(PivotMatrix, XMMatrixRotationY(XMConvertToRadians(180.0f)));
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BomberGreen"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/BomberGreen_Anim.bin"), PivotMatrix))))
				return E_FAIL;	

			/* For.Prototype_Component_Model_Cucco */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cucco"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/Cucco_Anim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_Rasengan */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rasengan"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Binary/NonAnim/Rasengan_NonAnim.bin"), PivotMatrix))))
				return E_FAIL;

			/* For.Prototype_Component_Model_ChainIronBall */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ChainIronBall"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Binary/NonAnim/ChainIronBall_NonAnim.bin"), PivotMatrix))))
				return E_FAIL; 

			/* For.Prototype_Component_Model_MiniGameA */
			{
				_matrix NewPivotMatrix = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixRotationY(XMConvertToRadians(180.0f)));
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MiniGameA"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Binary/NonAnim/MiniGameA_NonAnim.bin"), NewPivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Albatoss */
				NewPivotMatrix = XMMatrixIdentity();
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Albatoss"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/Albatoss_Anim.bin"), NewPivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_BonePutter */
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BonePutter"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/BonePutter_Anim.bin"), NewPivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_FlyingOctorok */
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlyingOctorok"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/FlyingOctorok_Anim.bin"), NewPivotMatrix))))
					return E_FAIL;

				/* For.Prototype_Component_Model_Piccolo */
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Piccolo"),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../Bin/Resources/Binary/Anim/Piccolo_Anim.bin"), NewPivotMatrix))))
					return E_FAIL;

			}
			

			/* For.Prototype_Component_Model_Boss_Map */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boss_Map"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Binary/NonAnim/DreamShrine_NonAnim.bin"), PivotMatrix))))
				return E_FAIL;
		}
		

		// Navi 네비게이션 로딩 //
		{
			m_strLoadingText = TEXT("네비게이션를(을) 로딩 중 입니다.");
			/* For.Prototype_Component_Navigation */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviMesh.dat")))))
				return E_FAIL;

			/* For.Prototype_Component_Navigation_MiniGameA */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_MiniGameA"),
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviMesh_MiniGameA.dat")))))
				return E_FAIL;

			/* For.Prototype_Component_Navigation_Boss_Map */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Boss_Map"),
				CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NaviMesh_Boss_Map.dat")))))
				return E_FAIL;
		}
		

		// Collider 콜라이더 로딩 //
		{
			m_strLoadingText = TEXT("충돌체를(을) 로딩 중 입니다.");
			/* For.Prototype_Component_Collider_AABB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
				return E_FAIL;

			/* For.Prototype_Component_Collider_Sphere */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
				return E_FAIL;

			/* For.Prototype_Component_Collider_OBB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
				return E_FAIL;

			/* For.Prototype_Component_Collider_Sword_OBB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sword_OBB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
				return E_FAIL;

			/* For.Prototype_Component_Collider_Shield_OBB */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Shield_OBB"),
				CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
				return E_FAIL;
		}
		

		// Shader 셰이더 로딩 //
		{
			m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
			/* For.Prototype_Component_Shader_VtxPosTex*/
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxNorTex*/
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxMesh */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxAnimMesh */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxCube */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxRectInstance */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTX_RECT_INSTANCE::Elements, VTX_RECT_INSTANCE::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxPointInstance */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::Elements, VTX_POINT_INSTANCE::iNumElements))))
				return E_FAIL;

			/* For.Prototype_Component_Shader_VtxAnimTexture */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimTexture"),
				CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimTexture.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
				return E_FAIL;
		}
		
	}

	/////////////////////////
	// GameObject 객체 로딩//
	/////////////////////////
	{
		m_strLoadingText = TEXT("객체원형를(을) 로딩 중 입니다.");

		/* Effect 이펙트 */
		{
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Texture"),
				CEffect_Texture::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}
		/* SkyBox 스카이박스 */
		{
			/* For Prototype_GameObject_Sky */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
				CSky::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}

		/* MiniGame 콜라이더 박스 */
		{
			// MiniGameA
			{
				/* For Prototype_GameObject_Map_GameA */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_GameA"),
					CMap_GameA::Create(m_pDevice, m_pContext))))
					return E_FAIL;


				/* For Prototype_GameObject_MiniGameA */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA"),
					CMini_GameA::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_MiniGameA_Back */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA_Back"),
					CMini_GameA_Back::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_MiniGameA */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA_Albatoss"),
					CAlbatoss::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_MiniGameA */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA_BonePutter"),
					CBonePutter::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_MiniGameA_FlyingOctorok */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA_FlyingOctorok"),
					CFlyingOctorok::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_MiniGameA */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameA_Piccolo"),
					CPiccolo::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			


			}
			

			// MiniGameB
			{
				///* For Prototype_GameObject_Map_GameB */
				//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_GameB"),
				//	CMap_GameB::Create(m_pDevice, m_pContext))))
				//	return E_FAIL;

				/* For Prototype_GameObject_MiniGameB */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MiniGameB"),
					CMini_GameB::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
			
		}

		// Camera 카메라 //
		{
			/* For Prototype_GameObject_Camera_Free */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
				CCamera_Free::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}

		// Map 맵 //
		{
			/* For Prototype_GameObject_MapOBJ*/
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapOBJ"),
				CMapObject::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_Nav */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Nav"),
				CNavMesh::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}

		// Particle 파티클 //
		{
			/* For Prototype_GameObject_Particle_Rect */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Rect"),
				CParticle_Rect::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For.Prototype_GameObject_Particle_Point */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Point"),
				CParticle_Point::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}

		// Player 플레이어 //
		{
			/* For.Prototype_GameObject_Player */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
				CPlayer::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_Body_Player */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
				CBody_Player::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_Weapon */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
				CWeapon::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_Weapon */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_FireBall"),
				CFireBall::Create(m_pDevice, m_pContext))))
				return E_FAIL;
		}
		
		// Monster 몬스터 //
		{
			/* For.Prototype_GameObject_Monster */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_BlueMoriblin"),
				CBlueMoriblin::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Zombie"),
				CZombie::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_StalfosOrange"),
				CStalfosOrange::Create(m_pDevice, m_pContext))))
				return E_FAIL;

		}

		// Boss Monster 보스 몬스터 //
		{
			/* For Prototype_GameObject_Boss_Stage */
			{
				/* For Prototype_GameObject_Boss_Map */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Map"),
					CBoss_Map::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_Collider_MainToBoss */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collider_MainToBoss"),
					CCollider_MainToBoss::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_Collider_BossToMain */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collider_BossToMain"),
					CCollider_BossToMain::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
		
			/* For Prototype_GameObject_PotDemon */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon"),
				CPotDemon::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_PotDemon_Pot */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_Pot"),
				CPotDemonPot::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			/* For Prototype_GameObject_PotDemon_King */
			if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_King"),
				CPotDemonKing::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			// 보스 공
			{
				/* For Prototype_GameObject_BomberGreen */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_BomberGreen"),
					CBomberGreen::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_ChainIronBall */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_ChainIronBall"),
					CChainIronBall::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_Cucco */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_Cucco"),
					CCucco::Create(m_pDevice, m_pContext))))
					return E_FAIL;

				/* For Prototype_GameObject_Rasengan */
				if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PotDemon_Rasengan"),
					CRasengan::Create(m_pDevice, m_pContext))))
					return E_FAIL;

			}
		}
	}
	
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MiniGameA()
{
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MiniGameB()
{
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_bIsFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL _eNextLevel)
{
	CLoader* pInstance = new CLoader(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{	
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

