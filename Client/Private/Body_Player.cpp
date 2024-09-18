#include "stdafx.h"
#include "Body_Player.h"

#include "Player.h"

// Engine
#include "Animation.h"
#include "Effect_Texture.h"

CBody_Player::CBody_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CBody_Player::CBody_Player(const CBody_Player& _rhs)
	: CPartObject(_rhs)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* _pArg)
{
	BODY_PLAYER_DESC* pBodyPlayerDesc = (BODY_PLAYER_DESC*)_pArg;
	{
		m_pPlayerState = pBodyPlayerDesc->pPlayerState;
		m_bChargingGauge = pBodyPlayerDesc->bChargingGauge;
		m_bUltJump = pBodyPlayerDesc->bUltJump;
		m_bUltST = pBodyPlayerDesc->bUltST;
		m_bUltLP = pBodyPlayerDesc->bUltLP;
		m_bUltED = pBodyPlayerDesc->bUltED;
		m_bIsSpecialSlash = pBodyPlayerDesc->bIsSpecialSlash;

		m_bCancelOnTerrain = pBodyPlayerDesc->bCancelOnTerrain;
	}

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pModelCom->Set_Init_Animation(0);

	return S_OK;
}

void CBody_Player::Priority_Tick(_float _fTimeDelta)
{
	Invalidate_ColliderTransformationMatrix();
}

void CBody_Player::Tick(_float _fTimeDelta)
{
	// 아이들
	{
		if (*m_pPlayerState == STATE_IDLE)
		{
			m_pModelCom->Set_Animation(0);
			m_pModelCom->Play_Animation(_fTimeDelta, true);
			return;
		}

		if (*m_pPlayerState == STATE_IDLE_CARRY)
			m_pModelCom->Set_Animation(1);
	}

	// 토킹
	{
		if (*m_pPlayerState == STATE_TALK_BACK)
			m_pModelCom->Set_Animation(2);

		if (*m_pPlayerState == STATE_TALK)
			m_pModelCom->Set_Animation(3);
	}
	

	// 걷기 & 뛰기
	{
		if (*m_pPlayerState == STATE_WALK)
		{
			m_pModelCom->Set_Animation(4);
			m_pModelCom->Get_VecAnims()[4]->Set_TickPerSec(50.f);
		}

		if (*m_pPlayerState == STATE_RUN)
			m_pModelCom->Set_Animation(5);
	}
	

	// 쉴드
	{
		if (*m_pPlayerState == STATE_SHIELD)
		{
			m_pModelCom->Set_Animation(6);
			m_pModelCom->Get_VecAnims()[6]->Set_TickPerSec(80.f);
		}
		if (*m_pPlayerState == STATE_SHIELD_F)
			m_pModelCom->Set_Animation(7);

		if (*m_pPlayerState == STATE_SHIELD_B)
			m_pModelCom->Set_Animation(8);

		if (*m_pPlayerState == STATE_SHIELD_L)
			m_pModelCom->Set_Animation(9);

		if (*m_pPlayerState == STATE_SHIELD_R)
			m_pModelCom->Set_Animation(10);

		if (*m_pPlayerState == STATE_SHIELD_HIT)
			m_pModelCom->Set_Animation(11);
	}
	


	// 슬래쉬
	{
		if (*m_pPlayerState == STATE_SLASH)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[12]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[12]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				*m_pPlayerState = STATE_END;
			}

			m_pModelCom->Set_Animation(12);
			m_pModelCom->Get_VecAnims()[12]->Set_TickPerSec(50.f);

			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		if (*m_pPlayerState == STATE_SLASH_LP)
			m_pModelCom->Set_Animation(13);

		if (*m_pPlayerState == STATE_SLASH_F)
			m_pModelCom->Set_Animation(14);

		if (*m_pPlayerState == STATE_SLASH_B)
			m_pModelCom->Set_Animation(15);

		if (*m_pPlayerState == STATE_SLASH_L)
			m_pModelCom->Set_Animation(16);

		if (*m_pPlayerState == STATE_SLASH_R)
			m_pModelCom->Set_Animation(17);

		if (*m_pPlayerState == STATE_NORMAL_SLASH_ED)
		{
			_double dDuration= m_pModelCom->Get_VecAnims()[18]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[18]->Get_TrackPosition();

			if (dTrackPosition >= 30)
			{
				(*m_bIsSpecialSlash) = false;
				*m_pPlayerState = STATE_END;
			}

			m_pModelCom->Set_Animation(18);
			m_pModelCom->Get_VecAnims()[18]->Set_TickPerSec(50.f);

			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		if (*m_pPlayerState == STATE_SPECIAL_SLASH_ED)
			m_pModelCom->Set_Animation(19);
			
	}
	


	// 슬래쉬 & 쉴드
	if (*m_pPlayerState == STATE_SLASH_SHIELD)
		m_pModelCom->Set_Animation(20);

	if (*m_pPlayerState == STATE_SLASH_SHIELD_F)
		m_pModelCom->Set_Animation(21);

	if (*m_pPlayerState == STATE_SLASH_SHIELD_B)
		m_pModelCom->Set_Animation(22);

	if (*m_pPlayerState == STATE_SLASH_SHIELD_L)
		m_pModelCom->Set_Animation(23);

	if (*m_pPlayerState == STATE_SLASH_SHIELD_R)
		m_pModelCom->Set_Animation(24);


	// 푸쉬 & 풀
	if (*m_pPlayerState == STATE_PUSH)
		m_pModelCom->Set_Animation(25);

	if (*m_pPlayerState == STATE_PULL)
		m_pModelCom->Set_Animation(26);


	// 아이템 갯 모션
	if (*m_pPlayerState == STATE_ITEM_ST)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[27]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[27]->Get_TrackPosition();

		if (dTrackPosition >= dDuration)
		{
			*m_pPlayerState = STATE_END;
		}

		m_pModelCom->Set_Animation(27);

		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}

	if (*m_pPlayerState == STATE_Lv25_SWRODGET_ST)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[28]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[28]->Get_TrackPosition();

		if (dTrackPosition >= dDuration)
		{
			(*m_pPlayerState) = STATE_JUMP;
			(*m_bUltJump) = true;
		}

		m_pModelCom->Set_Animation(28);
		m_pModelCom->Get_VecAnims()[28]->Set_TickPerSec(75);

		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}
		

	// 점프
	if (*m_pPlayerState == STATE_JUMP)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[29]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[29]->Get_TrackPosition();

		if (dTrackPosition >= dDuration && (*m_bCancelOnTerrain))
		{
			*m_pPlayerState = STATE_DASH_ST;
			(*m_bUltJump) = false;
			(*m_bUltST) = true;
		}
		
		if ((dTrackPosition >= dDuration) && (!(*m_bCancelOnTerrain)))
		{
			*m_pPlayerState = STATE_END;
		}

		m_pModelCom->Set_Animation(29);
		m_pModelCom->Get_VecAnims()[29]->Set_TickPerSec(75.f);
		
		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}

	// 뒤로 데쉬
	if (*m_pPlayerState == STATE_DASH_CRASH)
		m_pModelCom->Set_Animation(30);


	// 데미지 입었을때
	if (*m_pPlayerState == STATE_DMG_ELEC)
		m_pModelCom->Set_Animation(31);

	if (*m_pPlayerState == STATE_DMG_QUAKE)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[32]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[32]->Get_TrackPosition();

		if (dTrackPosition >= dDuration)
		{
			*m_pPlayerState = STATE_END;
		}

		m_pModelCom->Set_Animation(32);

		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}


	// 죽음, 부활
	if (*m_pPlayerState == STATE_DEATH)
		m_pModelCom->Set_Animation(33);

	if (*m_pPlayerState == STATE_RESURRECTION)
		m_pModelCom->Set_Animation(34);


	// 좌우 버튼, 키 열기
	if (*m_pPlayerState == STATE_BUTTON_L)
		m_pModelCom->Set_Animation(35);

	if (*m_pPlayerState == STATE_BUTTON_R)
		m_pModelCom->Set_Animation(36);

	if (*m_pPlayerState == STATE_KEY_OPEN)
		m_pModelCom->Set_Animation(37);

	
	// 필살기
	if (*m_pPlayerState == STATE_DASH_ST)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[38]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[38]->Get_TrackPosition();

		if (dTrackPosition >= dDuration)
		{
			*m_pPlayerState = STATE_DASH_LP;
			(*m_bUltST) = false;
			(*m_bUltLP) = true;
		}

		m_pModelCom->Set_Animation(38);

		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}
	
	if (*m_pPlayerState == STATE_DASH_LP)
	{
		m_pModelCom->Set_Animation(39);
		m_pModelCom->Play_Animation(_fTimeDelta, true);
		return;
	}

	if (*m_pPlayerState == STATE_DASH_ED)
	{
		_double dDuration = m_pModelCom->Get_VecAnims()[40]->Get_Duration();
		_double dTrackPosition = m_pModelCom->Get_VecAnims()[40]->Get_TrackPosition();

		if (dTrackPosition >= dDuration)
		{
			*m_pPlayerState = STATE_END;
			(*m_bCancelOnTerrain) = false;
			(*m_bUltED) = false;
		}

		m_pModelCom->Set_Animation(40);

		m_pModelCom->Play_Animation(_fTimeDelta, false);
		return;
	}

	// 들기 & 던지기
	if (*m_pPlayerState == STATE_CARRY)
		m_pModelCom->Set_Animation(41);

	if (*m_pPlayerState == STATE_LAND_CARRY)
		m_pModelCom->Set_Animation(42);

	if (*m_pPlayerState == STATE_JUMP_CARRY)
		m_pModelCom->Set_Animation(43);

	if (*m_pPlayerState == STATE_THROW)
		m_pModelCom->Set_Animation(44);

	*m_pPlayerState = STATE_END;

	m_pModelCom->Play_Animation(_fTimeDelta, true);
}

void CBody_Player::Late_Tick(_float _fTimeDelta)
{
	m_fTimeDelta = _fTimeDelta;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CBody_Player::Render()
{
	if ((*m_pPlayerState) == STATE_DMG_QUAKE)
	{
		m_fHitTime += m_fTimeDelta;

		if (m_fHitTime > m_fRenderTime)
		{
			m_fHitTime = 0.f;
			m_bRender = !m_bRender;
		}

		if (m_bRender)
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

			for (_uint i = 0; i < iNumMeshes; i++)
			{
				if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
					return E_FAIL;

				// 메시에게 영향을 주는 뼈들을 받아옴, 전체 뼈가 아닌,  뒤에 i는 그 몇번 메시 인지를 알기 위한 인자
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;

				if (FAILED(m_pModelCom->Render(i)))
					return E_FAIL;
			}
		}
	}
	else
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material_ShaderResource(m_pShaderCom, i, aiTextureType::aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			// 메시에게 영향을 주는 뼈들을 받아옴, 전체 뼈가 아닌,  뒤에 i는 그 몇번 메시 인지를 알기 위한 인자
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(i)))
				return E_FAIL;
		}
	}
	


#ifdef _DEBUG
	m_pColliderCom->Render();
#endif


	return S_OK;
}

void CBody_Player::Invalidate_ColliderTransformationMatrix()
{	
	// 내 자신의 월드행렬에 부모의 월드행렬을 곱함으로써, 최종 월드 행렬을 가진다
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

HRESULT CBody_Player::Add_Component()
{
	/* Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}
	
	/* Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Link"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}	

	/* Body Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		BodyBoundingDesc{};
		{
			BodyBoundingDesc.fRadius = 0.6f;
			BodyBoundingDesc.vCenter = _float3(0.f, BodyBoundingDesc.fRadius + 0.2f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BodyBoundingDesc)))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	// 조명 관련 쉐이더에 던지기
	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	// 받아온 카메라 위치 정보 쉐이더에 던지기
	_float4 CamPos = m_pGameInstance->Get_CamPosition();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &CamPos, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CBody_Player* CBody_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBody_Player* pInstance = new CBody_Player(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* _pArg, _uint _iLevel)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
