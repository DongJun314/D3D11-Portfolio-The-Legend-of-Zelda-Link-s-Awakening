#include "stdafx.h"
#include "PotDemonPot.h"

// Client
#include "Player.h"

// Engine
#include "Cell.h"
#include "Animation.h"

#include "Effect_Texture.h"

CPotDemonPot::CPotDemonPot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CPotDemonPartObject(_pDevice, _pContext)
{
}

CPotDemonPot::CPotDemonPot(const CPotDemonPot& _rhs)
    : CPotDemonPartObject(_rhs)
{
}

HRESULT CPotDemonPot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPotDemonPot::Initialize(void* _pArg)
{
    POT_DESC* pPotDesc = (POT_DESC*)_pArg;
    {
        m_pParentTransform = pPotDesc->pParentTransfrom;
        m_pPotDemonState = pPotDesc->pPotDemonState;
        m_pNavTransform = pPotDesc->pNavTranform;
        Safe_AddRef(m_pNavTransform);
        m_vecCells = pPotDesc->vecCells;


        m_bDemonKingComeOut = pPotDesc->bDemonKingComeOut;
        m_bWait = pPotDesc->bWait;

        m_bPop = pPotDesc->bPop;
        m_bDepop = pPotDesc->bDepop;

        m_bHit = pPotDesc->bHit;
        m_iHitCount = pPotDesc->iHitCount;

        m_bGetUp = pPotDesc->bGetUp;
        m_bLayDown = pPotDesc->bLayDown;

        m_bAtkFire = pPotDesc->bAtkFire;
        m_bAtkJuggling = pPotDesc->bAtkJuggling;
    }

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pEffect_Pot = dynamic_cast<CEffect_Texture*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),6));
    if (nullptr == m_pEffect_Pot)
        return E_FAIL;

    Safe_AddRef(m_pEffect_Pot);

    m_pModelCom->Set_Init_Animation(0);

    return S_OK;
}

void CPotDemonPot::Priority_Tick(_float _fTimeDelta)
{
    Invalidate_ColliderTransformationMatrix();
}

void CPotDemonPot::Tick(_float _fTimeDelta)
{
    if (*m_pPotDemonState == POT_WAIT)
        m_pModelCom->Set_Animation(0);

    if (*m_pPotDemonState == POT_WALK)
        m_pModelCom->Set_Animation(1);
    
    if (*m_pPotDemonState == POT_STUN)
    {
        _double dDuration = m_pModelCom->Get_VecAnims()[2]->Get_Duration();
        _double dTrackPosition = m_pModelCom->Get_VecAnims()[2]->Get_TrackPosition();

        if (dTrackPosition >= 45.f
            && (*m_iHitCount) < 3)
        {
            *m_bHit = false;
            ++(*m_iHitCount);
            *m_pPotDemonState = POT_WAIT;
        }

        if (dTrackPosition >= 45.f
            && (*m_iHitCount) == 3)
        {
            *m_bHit = false;
            *m_bLayDown = true;
            *m_iHitCount = 0;
        }

        m_pModelCom->Set_Animation(2);
        m_pModelCom->Play_Animation(_fTimeDelta, false);
        return;
    }

    if (*m_pPotDemonState == POT_STUN_WAIT)
    {
        m_pModelCom->Set_Animation(3);
        m_pModelCom->Play_Animation(_fTimeDelta, true);
        return;
    }
    
    if (*m_pPotDemonState == POT_GETUP)
    {
        _double dDuration = m_pModelCom->Get_VecAnims()[4]->Get_Duration();
        _double dTrackPosition = m_pModelCom->Get_VecAnims()[4]->Get_TrackPosition();

        if (dTrackPosition >= dDuration)
        {
            *m_bGetUp = false;
            *m_bPop = true;
        }

        m_pModelCom->Set_Animation(4);
        m_pModelCom->Play_Animation(_fTimeDelta, false);
        return;
    }
        

    if (*m_pPotDemonState == POT_POP)
    {
        m_pModelCom->Set_Animation(5);
        m_pModelCom->Play_Animation(_fTimeDelta, true);
        return;
    }
    
    if (*m_pPotDemonState == POT_ATTACK_JUGGLING)
    {
        m_pModelCom->Set_Animation(6);
        m_pModelCom->Play_Animation(_fTimeDelta, true);
        return;
    }

    if (*m_pPotDemonState == POT_ATTACK_FIRE)
    {
        m_pModelCom->Set_Animation(7);
        m_pModelCom->Play_Animation(_fTimeDelta, true);
        return;
    }
    
    if (*m_pPotDemonState == POT_DEPOP)
    {
      /*  _double dDuration = m_pModelCom->Get_VecAnims()[8]->Get_Duration();
        _double dTrackPosition = m_pModelCom->Get_VecAnims()[8]->Get_TrackPosition();

        if (dTrackPosition >= dDuration)
        {
            *m_bLayDown = true;
        }*/

        m_pModelCom->Set_Animation(8);
        m_pModelCom->Play_Animation(_fTimeDelta, false);
        return;
    }

    if (*m_pPotDemonState == POT_POP_WAIT)
    {
        m_pModelCom->Set_Animation(9);
        m_pModelCom->Play_Animation(_fTimeDelta, true);
        return;
    }

    m_pModelCom->Play_Animation(_fTimeDelta, true);
}

void CPotDemonPot::Late_Tick(_float _fTimeDelta)
{
    Collision_ToPlayer();

    //SetUp_OnTerrain(m_pTransformCom, m_pNavigationCom);

    if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
        return;
}

HRESULT CPotDemonPot::Render()
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
    m_pColliderCom->Render();
#endif


    return S_OK;
}

void CPotDemonPot::Collision_ToPlayer()
{
    _int iCurAnim = { -1 };
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    CModel* pModel = dynamic_cast<CModel*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Model")));
    _vector vPlayerPos = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

    // 몬스터 위치 
    //_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 이팩트 위치
    CTransform* pEffect_Pot = dynamic_cast<CTransform*>(m_pEffect_Pot->Get_Component(TEXT("Com_Transform")));// 이팩트 위치

    // 현재 플레이어의 애니메이션 상태를 가져옵니다
    if (nullptr != pModel)
        iCurAnim = pModel->Get_CurAnimation();

    if (Collision_ToPlayerSword()
        && (STATE_SLASH == iCurAnim
            || STATE_SLASH_LP == iCurAnim
            || STATE_NORMAL_SLASH_ED == iCurAnim)
        && (*m_bDemonKingComeOut) == false)
    {
        *m_bHit = true;

        m_pEffect_Pot->Set_EffectOnce(true);

        _vector vMidPos = {};
        {
            vMidPos.m128_f32[0] = m_WorldMatrix._41 - 0.2f * (m_WorldMatrix._41 - vPlayerPos.m128_f32[0]);
            vMidPos.m128_f32[1] = m_WorldMatrix._42 - 0.2f * (m_WorldMatrix._42 - vPlayerPos.m128_f32[1]) + 1.5f;
            vMidPos.m128_f32[2] = m_WorldMatrix._43 - 0.2f * (m_WorldMatrix._43 - vPlayerPos.m128_f32[2]);
        }

        pEffect_Pot->Set_State(CTransform::STATE_POSITION, vMidPos);
        m_pGameInstance->PlaySoundEx("Pot_Damage.wav", SOUNDTYPE::MON_DAMAGE, 0.4f);
    }

    if (m_pEffect_Pot->Get_EffectOnce())
    {
        m_pEffect_Pot->Animate_Texture(false);
    }
}

_bool CPotDemonPot::Collision_ToPlayerBody()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Body"), TEXT("Com_Collider")));
    if (nullptr == pPlayerCollider)
        return false;

    return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CPotDemonPot::Collision_ToPlayerSword()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    if (nullptr == pPlayer)
        return false;

    CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayer->Get_PartObjectComponent(TEXT("Part_Weapon"), TEXT("Com_Sword_Collider")));
    if (nullptr == pPlayerCollider)
        return false;

    return m_pColliderCom->Intersect(pPlayerCollider);
}

_bool CPotDemonPot::Collision_ToPlayerShield()
{
    return _bool();
}

void CPotDemonPot::Invalidate_ColliderTransformationMatrix()
{
    // 내 자신의 월드행렬에 부모의 월드행렬을 곱함으로써, 최종 월드 행렬을 가진다
    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());

    m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

HRESULT CPotDemonPot::SetUp_OnTerrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
{
    // 높이 맵 적용이 필요한 객체의 Transform 위치를 얻어온다(현재 월드 상태)
    _vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE_POSITION);
    {
    	// 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
    	vTargetPos = XMVector3TransformCoord(vTargetPos, m_pNavTransform->Get_WorldMatrix_Inverse());

    	// 해당 객체의 Y축 좌표를 계산해서 반환한다 
    	_int iCellIndex = { -1 };

    	if (_pNavigation->Is_Move(vTargetPos))
    	{
    		iCellIndex = _pNavigation->Get_CellIndex();
    		vTargetPos = XMVectorSetY(vTargetPos, m_vecCells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vTargetPos));
    	}

    	// 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
    	_pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pNavTransform->Get_WorldMatrix()));
    }

    return S_OK;
}

HRESULT CPotDemonPot::Add_Component()
{
    /* Com_Shader */
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
            TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
            return E_FAIL;
    }

    /* Com_Model */
    {
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PotDemon_Pot"),
            TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;

    }

    /* Com_Navigation */
	CNavigation::NAVI_DESC NaviDesc{};
	{
		NaviDesc.iStartCellIndex = 225;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

    /* Body Com_Collider */
    {
        CBounding_Sphere::SPHERE_DESC		PotDemonBoundingDesc{};
        {
            PotDemonBoundingDesc.fRadius = 0.6f;
            PotDemonBoundingDesc.vCenter = _float3(0.f, PotDemonBoundingDesc.fRadius, 0.f);
        }

        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
            TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &PotDemonBoundingDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPotDemonPot::Bind_ShaderResources()
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

CPotDemonPot* CPotDemonPot::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPotDemonPot* pInstance = new CPotDemonPot(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPotDemonPot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPotDemonPot::Clone(void* _pArg, _uint _iLevel)
{
    CPotDemonPot* pInstance = new CPotDemonPot(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPotDemonPot");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPotDemonPot::Free()
{
    __super::Free();

    
   // Safe_Release(m_pNavigationCom);
    //Safe_Release(m_pNavTransform);
    Safe_Release(m_pEffect_Pot);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
