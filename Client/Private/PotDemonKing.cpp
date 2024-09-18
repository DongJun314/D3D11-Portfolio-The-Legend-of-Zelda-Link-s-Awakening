#include "stdafx.h"
#include "PotDemonKing.h"

#include "Model.h"
#include "Animation.h"

CPotDemonKing::CPotDemonKing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPotDemonPartObject(_pDevice, _pContext)
{
}

CPotDemonKing::CPotDemonKing(const CPotDemonKing& _rhs)
	: CPotDemonPartObject(_rhs)
{
}

HRESULT CPotDemonKing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPotDemonKing::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	DEMON_KING_DESC* pDemonKingDesc = (DEMON_KING_DESC*)_pArg;
	{
		m_iDemonKingState = pDemonKingDesc->iDemonKingState;

		// 몬스터 혹은 플레이어가 위에 있는 쪽으로 몸을 회전시키기 위한 함수 
		m_pSocketMatrix[DKC_ROOT] = pDemonKingDesc->pSocketMatrix[DKC_ROOT];
		m_pSocketMatrix[DKC_HAND_L] = m_pModelCom->Get_CombinedBoneMatrixPtr("hand_L");
		m_pSocketMatrix[DKC_HAND_R] = m_pModelCom->Get_CombinedBoneMatrixPtr("hand_R");
		m_pSocketMatrix[DKC_FB_A] = m_pModelCom->Get_CombinedBoneMatrixPtr("fire_a");
		m_pSocketMatrix[DKC_FB_B] = m_pModelCom->Get_CombinedBoneMatrixPtr("fire_b");
		m_pSocketMatrix[DKC_FB_C] = m_pModelCom->Get_CombinedBoneMatrixPtr("fire_c");

		m_bHit = pDemonKingDesc->bHit;
		m_iHitCount = pDemonKingDesc->iHitCount;

		m_bWait = pDemonKingDesc->bWait;
		m_bPop = pDemonKingDesc->bPop;
		m_bAtkJuggling = pDemonKingDesc->bAtkJuggling;
		m_bAtkFire = pDemonKingDesc->bAtkFire;
		m_bAtkUlt = pDemonKingDesc->bAtkUlt;
		m_bDepop = pDemonKingDesc->bDepop;
		m_bDead = pDemonKingDesc->bDead;

		// Demon King
		m_bPhase2Pop = pDemonKingDesc->bPhase2Pop;
		m_bPhase2Wait = pDemonKingDesc->bPhase2Wait;
		m_bPhase2Disappear = pDemonKingDesc->bPhase2Disappear;
		m_bPhase2Appear = pDemonKingDesc->bPhase2Appear;
		m_bPhase2Punch = pDemonKingDesc->bPhase2Punch;
		m_bPhase2PunchED = pDemonKingDesc->bPhase2PunchED;
		m_bPhase2ThrowST = pDemonKingDesc->bPhase2ThrowST;
		m_bPhase2ThrowLP = pDemonKingDesc->bPhase2ThrowLP;
		m_bPhase2ThrowED = pDemonKingDesc->bPhase2ThrowED;
		m_bPhase2Damage = pDemonKingDesc->bPhase2Damage;
		m_bPhase2Dead = pDemonKingDesc->bPhase2Dead;


		m_bOnFireCucco = pDemonKingDesc->bOnFireCucco;
		m_bOnFireBomberGreen = pDemonKingDesc->bOnFireBomberGreen;
		m_bOnFireChainIronBall = pDemonKingDesc->bOnFireChainIronBall;
		m_bOnFireRasengan = pDemonKingDesc->bOnFireRasengan;

		m_iCucco_CloneCount = pDemonKingDesc->iCucco_CloneCount;
		m_iChainIronBall_CloneCount = pDemonKingDesc->iChainIronBall_CloneCount;
		m_iBomberGreen_CloneCount = pDemonKingDesc->iBomberGreen_CloneCount;
		m_iRasengan_CloneCount = pDemonKingDesc->iRasengan_CloneCount;
	}

	if (*m_iDemonKingState != POTDEMON_STATE_END)
		m_pModelCom->Set_Init_Animation(0);

	return S_OK;
}

void CPotDemonKing::Priority_Tick(_float _fTimeDelta)
{
	// 월드 좌표 세팅
	{
		CGameObject* pGameObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		_vector vPlayerPos = dynamic_cast<CTransform*>(pGameObject->Get_Component((TEXT("Com_Transform"))))->Get_State(CTransform::STATE_POSITION);

		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMLoadFloat4x4(m_pSocketMatrix[DKC_ROOT]) * m_pParentTransform->Get_WorldMatrix());

		_vector vLook = XMVector3Normalize(vPlayerPos - _vector{ WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f });
		_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		_vector vUp = XMVector3Cross(vLook, vRight);

		_float vScaled = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&WorldMatrix).r[0]));

		_float4 vTmp, vDst, vSrc;
		XMStoreFloat4(&vTmp, XMVector3Normalize(vRight) * vScaled);
		memcpy(&WorldMatrix.m[0], &vTmp, sizeof(_float4));

		XMStoreFloat4(&vDst, XMVector3Normalize(vUp) * vScaled);
		memcpy(&WorldMatrix.m[1], &vDst, sizeof(_float4));

		XMStoreFloat4(&vSrc, XMVector3Normalize(vLook) * vScaled);
		memcpy(&WorldMatrix.m[2], &vSrc, sizeof(_float4));

		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&WorldMatrix));
		m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
	}

	Invalidate_ColliderTransformationMatrix();
}

void CPotDemonKing::Tick(_float _fTimeDelta)
{
	// Phase 1
	{
		if (*m_iDemonKingState == KING_POP)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[0]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[0]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				*m_bPop = false;
				*m_bWait = true;
			}

			m_pModelCom->Set_Animation(0);

			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		if (*m_iDemonKingState == KING_WAIT)
		{
			m_pModelCom->Set_Animation(1);
			m_pModelCom->Play_Animation(_fTimeDelta, true);
			return;
		}

		if (*m_iDemonKingState == KING_ATTACK_JUGGLING)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[2]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[2]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				*m_bAtkJuggling = false;
				*m_bAtkFire = true;
			}

			m_pModelCom->Set_Animation(2);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		// 이건 순간이동을 사용할시에 사용되는 애니메이션이다
		//if (*m_iDemonKingState == KING_ATTACK_FIRE)
		//{
		//	_double dDuration = m_pModelCom->Get_VecAnims()[3]->Get_Duration();
		//	_double dTrackPosition = m_pModelCom->Get_VecAnims()[3]->Get_TrackPosition();

		//	// a = Bomber,  b = ChainBall, c = Cucco
		//	// 69에서 fire b 발사
		//	// 109에서 fire c 발사
		//	// 147에서 fire a/c발사 
		//	// 187에서 fire a발사
		//	// 225에서 fire a/b 발사
		//	// 265에서 fire b 발사
		//	// 301에서 fire b/c 발사
		//	// 343에서 fire c 발사


		//	if (dTrackPosition >= 69 && dTrackPosition <= 72)
		//	{
		//		(*m_bOnFireChainIronBall) = true;
		//	}

		//	if (dTrackPosition >= 109 && dTrackPosition <= 112)
		//	{
		//		(*m_bOnFireCucco) = true;
		//	}

		//	if (dTrackPosition >= 147 && dTrackPosition <= 150)
		//	{
		//		(*m_bOnFireBomberGreen) = true;
		//	}

		//	if (dTrackPosition >= 187 && dTrackPosition <= 190)
		//	{
		//		(*m_bOnFireBomberGreen) = true;
		//	}

		//	if (dTrackPosition >= 225 && dTrackPosition <= 228)
		//	{
		//		(*m_bOnFireBomberGreen) = true;
		//	}

		//	if (dTrackPosition >= 265 && dTrackPosition <= 268)
		//	{
		//		(*m_bOnFireChainIronBall) = true;
		//	}

		//	if (dTrackPosition >= 301 && dTrackPosition <= 304)
		//	{
		//		(*m_bOnFireCucco) = true;
		//	}

		//	if (dTrackPosition >= 343 && dTrackPosition <= 346)
		//	{
		//		(*m_bOnFireCucco) = true;
		//	}

		//	if (dTrackPosition >= dDuration)
		//	{
		//		*m_bAtkFire = false;
		//		*m_bAtkJuggling = true;
		//	}

		//	if ((*m_iHitCount) == 3)
		//	{
		//		*m_bAtkFire = false;

		//	}

		//	if ((*m_iHitCount) == 5)
		//	{
		//		*m_bAtkFire = false;
		//		*m_bDepop = true;
		//	}


		//	m_pModelCom->Set_Animation(3);
		//	m_pModelCom->Play_Animation(_fTimeDelta, true);
		//	return;
		//}

		//if (*m_iDemonKingState == KING_DEPOP)
		//{
		//	m_pModelCom->Set_Animation(4);
		//	m_pModelCom->Play_Animation(_fTimeDelta, false);
		//	return;
		//}

		if (*m_iDemonKingState == KING_ATTACK_FIRE)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[3]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[3]->Get_TrackPosition();

			// a = Bomber,  b = ChainBall, c = Cucco
			// 69에서 fire b 발사
			// 109에서 fire c 발사
			// 147에서 fire a/c발사 
			// 187에서 fire a발사
			// 225에서 fire a/b 발사
			// 265에서 fire b 발사
			// 301에서 fire b/c 발사
			// 343에서 fire c 발사


			if (dTrackPosition >= 69 && dTrackPosition <= 72)
			{
				if (!m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("PotDemon_Throw.mp3", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iChainIronBall_CloneCount)++;
					(*m_bOnFireChainIronBall) = true;
					m_bThrowOnce = true;
				}
			}

			if (dTrackPosition >= 109 && dTrackPosition <= 112)
			{
				if (m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("JugglingBalls.wav", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iCucco_CloneCount)++;
					(*m_bOnFireCucco) = true;
					m_bThrowOnce = false;
				}
			}

			if (dTrackPosition >= 147 && dTrackPosition <= 150)
			{
				if (!m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("PotDemon_Throw.mp3", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iBomberGreen_CloneCount) = 1;
					(*m_bOnFireBomberGreen) = true;
					m_bThrowOnce = true;
				}
			}

			if (dTrackPosition >= 187 && dTrackPosition <= 190)
			{
				if (m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("PotDemon_Throw.mp3", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iBomberGreen_CloneCount) = 1;
					(*m_bOnFireBomberGreen) = true;
					m_bThrowOnce = false;
				}
			}

			if (dTrackPosition >= 225 && dTrackPosition <= 228)
			{
				if (!m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("JugglingBalls.wav", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iBomberGreen_CloneCount) = 1;
					(*m_bOnFireBomberGreen) = true;
					m_bThrowOnce = true;
				}
			}

			if (dTrackPosition >= 265 && dTrackPosition <= 268)
			{
				if (m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("PotDemon_Throw.mp3", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iChainIronBall_CloneCount)++;
					(*m_bOnFireChainIronBall) = true;
					m_bThrowOnce = false;
				}
			}

			if (dTrackPosition >= 301 && dTrackPosition <= 304)
			{
				if (!m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("JugglingBalls.wav", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iCucco_CloneCount)++;
					(*m_bOnFireCucco) = true;
					m_bThrowOnce = true;
				}
			}

			if (dTrackPosition >= 343 && dTrackPosition <= 346)
			{
				if (m_bThrowOnce)
				{
					m_pGameInstance->PlaySoundEx("PotDemon_Throw.mp3", SOUNDTYPE::MON_ATK, 0.5f);
					(*m_iCucco_CloneCount)++;
					(*m_bOnFireCucco) = true;
					m_bThrowOnce = false;
				}
			}

			/*if (dTrackPosition >= dDuration)
			{
				*m_bAtkFire = false;
				*m_bAtkJuggling = true;
			}*/

			

			m_pModelCom->Set_Animation(3);
			m_pModelCom->Play_Animation(_fTimeDelta, true);
			return;
		}

		if (*m_iDemonKingState == KING_DEPOP)
		{
			m_pModelCom->Set_Animation(4);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}
	}
	

	// Phase 2
	{
		// 페이즈 2 나타나기
		if (*m_iDemonKingState == KING_PHASE_2_ST)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[5]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[5]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				(*m_bPhase2Wait) = true;
				(*m_bPhase2Pop) = false;
			}

			m_pModelCom->Set_Animation(5);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		// 페이즈 2 아이들 상태
		if (*m_iDemonKingState == KING_PHASE_2_WAIT)
		{
			m_pModelCom->Set_Animation(6);
			m_pModelCom->Play_Animation(_fTimeDelta, true);
			return;
		}

		// 공격을 위해 사라지기
		if (*m_iDemonKingState == KING_PHASE_2_ED)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[7]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[7]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				(*m_bPhase2Disappear) = false;
				(*m_bPhase2Appear) = true;
			}

			m_pModelCom->Set_Animation(7);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		// 공격을 위해 나타나기
		if (*m_iDemonKingState == KING_CLONING_APPEAR)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[8]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[8]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				(*m_bPhase2Appear) = false;
				(*m_bPhase2Punch) = true;
			}

			m_pModelCom->Set_Animation(8);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}

		// 근거리 난사 펀치
		{
			if (*m_iDemonKingState == KING_CLONING_ATK_PUNCH)
			{
				m_pModelCom->Set_Animation(9);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}

			if (*m_iDemonKingState == KING_CLONING_ATK_PUNCH_ED)
			{
				_double dDuration = m_pModelCom->Get_VecAnims()[10]->Get_Duration();
				_double dTrackPosition = m_pModelCom->Get_VecAnims()[10]->Get_TrackPosition();

				if (dTrackPosition >= dDuration)
				{
					(*m_bPhase2PunchED) = false;
					(*m_bPhase2Appear) = true;
					//(*m_bPhase2ThrowST) = true;
				}

				m_pModelCom->Set_Animation(10);
				m_pModelCom->Play_Animation(_fTimeDelta, false);
				return;
			}
		}
		

		// 원기옥 던지기
		{
			if (*m_iDemonKingState == KING_CLONING_ED_WAIT)
			{
				_double dDuration = m_pModelCom->Get_VecAnims()[11]->Get_Duration();
				_double dTrackPosition = m_pModelCom->Get_VecAnims()[11]->Get_TrackPosition();

				if (dTrackPosition >= dDuration)
				{
					(*m_bPhase2ThrowST) = false;
					(*m_bPhase2ThrowLP) = true;
				}



				m_pModelCom->Set_Animation(11);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}

			if (*m_iDemonKingState == KING_CLONING_ED_THROW)
			{
				_double dDuration = m_pModelCom->Get_VecAnims()[12]->Get_Duration();
				_double dTrackPosition = m_pModelCom->Get_VecAnims()[12]->Get_TrackPosition();

				if (dTrackPosition >= dDuration)
				{
					(*m_bPhase2ThrowLP) = false;
					(*m_bPhase2ThrowED) = true;
				}

				if (dTrackPosition > 6)
				{
					(*m_bOnFireRasengan) = true;
				}

				m_pModelCom->Set_Animation(12);
				m_pModelCom->Play_Animation(_fTimeDelta, false);
				return;
			}

			if (*m_iDemonKingState == KING_CLONING_ED_WAIT_2)
			{
				m_pModelCom->Set_Animation(13);
				m_pModelCom->Play_Animation(_fTimeDelta, true);
				return;
			}
		}
		

		if (*m_iDemonKingState == KING_CLONING_DAMAGE)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[14]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[14]->Get_TrackPosition();

			if (dTrackPosition >= 60)
			{
				*m_bHit = false;
				*m_bAtkFire = true;
				(*m_iHitCount)++;
			}
			if ((*m_iHitCount) == 3)
			{
			 	*m_bHit = false;
				*m_bAtkFire = false;
				*m_bDepop = true;
				(*m_iHitCount) = 0;
			}

			m_pModelCom->Set_Animation(14);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}


		if (*m_iDemonKingState == KING_CLONING_ULT_DAMAGE)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[14]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[14]->Get_TrackPosition();

			if (dTrackPosition >= 60)
			{
				*m_bHit = false;
				*m_bPhase2ThrowST = true;
				(*m_iHitCount)++;
				m_pGameInstance->PlaySoundEx("Genie_WakeUp.mp3", SOUNDTYPE::MON_DAMAGE, 0.5f);
			}
		
			if ((*m_iHitCount) == 2)
			{
				*m_bPhase2ThrowED = false;
				*m_bDepop = true;
			}

			m_pModelCom->Set_Animation(14);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}



		if (*m_iDemonKingState == KING_CLONING_DEAD)
		{
			_double dDuration = m_pModelCom->Get_VecAnims()[15]->Get_Duration();
			_double dTrackPosition = m_pModelCom->Get_VecAnims()[15]->Get_TrackPosition();

			if (dTrackPosition >= dDuration)
			{
				*m_bDepop = false;
				*m_bDead = true;
				//(*m_iHitCount)++;
			}

			m_pModelCom->Set_Animation(15);
			m_pModelCom->Play_Animation(_fTimeDelta, false);
			return;
		}
	}


	if(*m_iDemonKingState != POTDEMON_STATE_END)
		m_pModelCom->Play_Animation(_fTimeDelta, true);
}

void CPotDemonKing::Late_Tick(_float _fTimeDelta)
{
	Invalidate_ColliderTransformationMatrix();

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CPotDemonKing::Render()
{
	if (*m_iDemonKingState != POTDEMON_STATE_END)
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

#ifdef _DEBUG
		// 1 부터 시작하는 이유 : DKC_ROOT는 실제로 사용하기 위해서 받아온것이 아니기때문
		for (size_t i = 1; i < DKC_FB_A; i++)
		{
			m_pColliderCom[i]->Render();
		}
#endif
	}

	return S_OK;
}

void CPotDemonKing::Collision_ToPlayer()
{
}

_bool CPotDemonKing::Collision_ToPlayerBody()
{
	return _bool();
}

_bool CPotDemonKing::Collision_ToPlayerSword()
{
	return _bool();
}

_bool CPotDemonKing::Collision_ToPlayerShield()
{
	return _bool();
}

void CPotDemonKing::Invalidate_ColliderTransformationMatrix()
{
	///* Left Hand Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[DKC_HAND_L]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_pHand_L_WorldMatrix, BoneMatrix * XMLoadFloat4x4(&m_WorldMatrix)); // 보임

		m_pColliderCom[DKC_HAND_L]->Tick(XMLoadFloat4x4(&m_pHand_L_WorldMatrix));
	}

	/* Right Hand Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[DKC_HAND_R]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_pHand_R_WorldMatrix, BoneMatrix * XMLoadFloat4x4(&m_WorldMatrix));

		m_pColliderCom[DKC_HAND_R]->Tick(XMLoadFloat4x4(&m_pHand_R_WorldMatrix));
	}

	/* FireBall A Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[DKC_FB_A]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		//여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_pFire_A_WorldMatrix, BoneMatrix * XMLoadFloat4x4(&m_WorldMatrix));
	}

	/* FireBall B Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[DKC_FB_B]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		// 여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_pFire_B_WorldMatrix, BoneMatrix * XMLoadFloat4x4(&m_WorldMatrix));
	}

	/* FireBall C Collider */
	{
		_matrix BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix[DKC_FB_C]);

		for (size_t i = 0; i < 3; i++)
		{
			BoneMatrix.r[i] = XMVector3Normalize(BoneMatrix.r[i]);
		}

		//여기서 부모의 월드 행렬을 곱하기 전에, 우선 뼈의 붙여주는 작업이 필요한데, 그게 BoneMatrix 곱해주는 작업이다
		XMStoreFloat4x4(&m_pFire_C_WorldMatrix, BoneMatrix * XMLoadFloat4x4(&m_WorldMatrix));
	}
}

HRESULT CPotDemonKing::Add_Component()
{
	/* Com_Shader */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;
	}

	/* Com_Model */
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PotDemon_King"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

	}

	/* Left Hand Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		DemonKing_L_Hand_BoundingDesc{};
		{
			DemonKing_L_Hand_BoundingDesc.fRadius = 0.45f;
			DemonKing_L_Hand_BoundingDesc.vCenter = _float3(0.5f, DemonKing_L_Hand_BoundingDesc.fRadius -0.5f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_L_Hand1_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[DKC_HAND_L]), &DemonKing_L_Hand_BoundingDesc)))
			return E_FAIL;
	}


	/* Right Hand Com_Collider */
	{
		CBounding_Sphere::SPHERE_DESC		DemonKing_R_Hand_BoundingDesc{};
		{
			DemonKing_R_Hand_BoundingDesc.fRadius = 0.45f;
			DemonKing_R_Hand_BoundingDesc.vCenter = _float3(-0.5f, DemonKing_R_Hand_BoundingDesc.fRadius -0.5f, 0.f);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_R_Hand_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[DKC_HAND_R]), &DemonKing_R_Hand_BoundingDesc)))
			return E_FAIL;		
	}

	///* FireBall A Com_Collider */
	//{
	//	CBounding_Sphere::SPHERE_DESC		DemonKing_FB_A_BoundingDesc{};
	//	{
	//		DemonKing_FB_A_BoundingDesc.fRadius = 0.2f;
	//		DemonKing_FB_A_BoundingDesc.vCenter = _float3(0.f, DemonKing_FB_A_BoundingDesc.fRadius, 0.f);
	//	}

	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//		TEXT("Com_FB_A1_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[DKC_FB_A]), &DemonKing_FB_A_BoundingDesc)))
	//		return E_FAIL;
	//}

	///* FireBall B Com_Collider */
	//{
	//	CBounding_Sphere::SPHERE_DESC		DemonKing_FB_B_BoundingDesc{};
	//	{
	//		DemonKing_FB_B_BoundingDesc.fRadius = 0.2f;
	//		DemonKing_FB_B_BoundingDesc.vCenter = _float3(0.f, DemonKing_FB_B_BoundingDesc.fRadius, 0.f);
	//	}

	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//		TEXT("Com_FB_B1_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[DKC_FB_B]), &DemonKing_FB_B_BoundingDesc)))
	//		return E_FAIL;
	//}

	///* FireBall C Com_Collider */
	//{
	//	CBounding_Sphere::SPHERE_DESC		DemonKing_FB_C_BoundingDesc{};
	//	{
	//		DemonKing_FB_C_BoundingDesc.fRadius = 0.2f;
	//		DemonKing_FB_C_BoundingDesc.vCenter = _float3(0.f, DemonKing_FB_C_BoundingDesc.fRadius, 0.f);
	//	}

	//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
	//		TEXT("Com_FB_C1_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom[DKC_FB_C]), &DemonKing_FB_C_BoundingDesc)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CPotDemonKing::Bind_ShaderResources()
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

void CPotDemonKing::Handle_Spheres_Sliding(CGameObject* _pGameObject)
{
}

CPotDemonKing* CPotDemonKing::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPotDemonKing* pInstance = new CPotDemonKing(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPotDemonKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPotDemonKing::Clone(void* _pArg, _uint _iLevel)
{
	CPotDemonKing* pInstance = new CPotDemonKing(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CPotDemonKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPotDemonKing::Free()
{
	__super::Free();

	// Collider 순회하면서 지우기
	for (size_t i = 0; i < DKC_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
