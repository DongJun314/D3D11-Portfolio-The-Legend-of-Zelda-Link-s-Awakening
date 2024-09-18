#include "stdafx.h"
#include "Effect_Texture.h"

// Engine
#include "Player.h"
#include "GameInstance.h"

CEffect_Texture::CEffect_Texture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CEffect_Texture::CEffect_Texture(const CEffect_Texture& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CEffect_Texture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Texture::Initialize(void* _pArg, _uint _iLevel)
{
	CEffect_Texture::EFFECTEXTURE_DESC* tEffectType_Desc = (EFFECTEXTURE_DESC*)_pArg;
	{
		m_eEffectType = tEffectType_Desc->eEffectType;
	}

	CGameObject::GAMEOBJECT_DESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));
	{
		GameObjectDesc.iGameObjectData = 10;
		GameObjectDesc.fSpeedPerSec = 10.f;
		GameObjectDesc.fRotationPerSec = XMConvertToRadians(60.f);
	}

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;


	switch (m_eEffectType)
	{
	case ET_HIT_BLOOD:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 24; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 5; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 5, 5 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 13;
	}
	break;

	case ET_HIT_METAL:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 30; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 5; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 4, 5 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 11;
	}
	break;

	case ET_CHARGING:
	{

	}
	break;

	case ET_SLASH_ED:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 25; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 1; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 6, 7 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 9;
	}
	break;

	case ET_ULT_LAND:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 25; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 1; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 7, 7 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 3;
	}
	break;


	case ET_EXPLOSION:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 63; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 1; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 8, 6 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 7;
	}
	break;

	case ET_POT_HIT:
	{
		ZeroMemory(&m_tAnimTex_Desc, sizeof(ANIMTEXTURE_DESC));
		{
			m_tAnimTex_Desc.iFrameNow = 0;
			m_tAnimTex_Desc.iFrameStart = 0;
			m_tAnimTex_Desc.iFrameEnd = 80; // 총 개수
			m_tAnimTex_Desc.dwSpeed = 1; // 숫자가 크면 느려집니다 
			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();
			m_tAnimTex_Desc.uRowColumn = { 9, 7 };
			m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
			m_tAnimTex_Desc.vUV = { (_float)1 / (m_tAnimTex_Desc.uRowColumn.x + 1), (_float)1 / (m_tAnimTex_Desc.uRowColumn.y + 1) };
		}

		m_iTextureIndex = 10;
	}
	break;

	default:
		break;
	}
	
	return S_OK;
}

void CEffect_Texture::Priority_Tick(_float _fTimeDelta)
{
	
}

void CEffect_Texture::Tick(_float _fTimeDelta)
{
	Handle_EffectType();
}

void CEffect_Texture::Late_Tick(_float _fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;
}

HRESULT CEffect_Texture::Render()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	////////////////////////////////////////////////////////////////////

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TextureUV", &m_tAnimTex_Desc.vUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CurRowColumn", &m_tAnimTex_Desc.uCurRowColumn, sizeof(XMUINT2))))
		return E_FAIL;

	////////////////////////////////////////////////////////////////////
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture::Set_BillBoard()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (pPlayer == nullptr)
		return E_FAIL;

	_bool bIsFirstView = pPlayer->Get_FirstView();

	_matrix WorldMatrix, WorldMatrixX, WorldMatrixY;

	_double dAngleX, dAngleY;
	_float fRotationX, fRotationY;
	_float4 vCamPos = m_pGameInstance->Get_CamPosition();

	_float4 vPos = { 0.f ,0.f, 0.f, 0.f };
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (bIsFirstView)
	{
		dAngleY = atan2(vPos.x - vCamPos.x, vPos.z - vCamPos.z) * (180.0 / ((FLOAT)3.141592654f));
		// 라디안 값으로 취환합니다
		fRotationY = (_float)dAngleY * 0.0174532925f;

		WorldMatrixY = XMMatrixRotationY(fRotationY);
		WorldMatrix = WorldMatrixY;
	}
	else
	{
		dAngleX = atan2(vPos.y - vCamPos.y, vPos.z - vCamPos.z) * (180.0 / ((FLOAT)3.141592654f));
		// 라디안 값으로 취환합니다
		fRotationX = (_float)-dAngleX * 0.0174532925f;

		WorldMatrixX = XMMatrixRotationX(fRotationX);
		WorldMatrix = WorldMatrixX;
	}

	_matrix TranslationMatrix = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	WorldMatrix = XMMatrixMultiply(WorldMatrix, TranslationMatrix);

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CEffect_Texture::Handle_EffectType()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));

	_bool bFirstView = pPlayer->Get_FirstView();

	switch (m_eEffectType)
	{
	case ET_HIT_BLOOD:
	{
		if (FAILED(Set_BillBoard()))
			return E_FAIL;

		if(bFirstView)
			m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
		else
			m_pTransformCom->Set_Scaling(5.f, 5.f, 5.f);
	}
	break;

	case ET_HIT_METAL:
	{
		if (FAILED(Set_BillBoard()))
			return E_FAIL;

		if (bFirstView)
			m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
		else
			m_pTransformCom->Set_Scaling(5.f, 5.f, 5.f);
	}
	break;

	case ET_CHARGING:
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	}
	break;

	case ET_SLASH_ED:
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)); 

		if (bFirstView)
			m_pTransformCom->Set_Scaling(8.f,8.f, 8.f);
		else
			m_pTransformCom->Set_Scaling(8.f, 8.f, 8.f);
	}
	break;

	case ET_ULT_LAND:
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_pTransformCom->Set_Scaling(4.f, 4.f, 4.f);
	}
	break;

	case ET_EXPLOSION:
	{
		if (FAILED(Set_BillBoard()))
			return E_FAIL;

		if (bFirstView)
			m_pTransformCom->Set_Scaling(3.f, 3.f, 3.f);
		else
			m_pTransformCom->Set_Scaling(5.f, 5.f, 5.f);
	}
	break;

	case ET_POT_HIT:
	{
		if (FAILED(Set_BillBoard()))
			return E_FAIL;

		if (bFirstView)
			m_pTransformCom->Set_Scaling(2.f, 2.f, 2.f);
		else
			m_pTransformCom->Set_Scaling(4.f, 4.f, 4.f);
	}
	break;
	 
	default:
		break;
	}

	return S_OK;
}

void CEffect_Texture::Show_Texture()
{
	m_tAnimTex_Desc.iFrameNow;
	m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
}

void CEffect_Texture::Animate_Texture(_bool _bLoop)
{
	if (_bLoop)
	{
		if (m_tAnimTex_Desc.dwSpeed + m_tAnimTex_Desc.dwTime < GetTickCount64())
		{
			++m_tAnimTex_Desc.iFrameNow;
			++m_tAnimTex_Desc.uCurRowColumn.x;

			if (m_tAnimTex_Desc.uCurRowColumn.x > m_tAnimTex_Desc.uRowColumn.x)
			{
				m_tAnimTex_Desc.uCurRowColumn.x = 0;
				++m_tAnimTex_Desc.uCurRowColumn.y;
			}

			m_tAnimTex_Desc.uRowColumn.y = m_tAnimTex_Desc.iFrameEnd / (m_tAnimTex_Desc.uRowColumn.x + 1) - 1;

			if (m_tAnimTex_Desc.uCurRowColumn.y == m_tAnimTex_Desc.uRowColumn.y)
			{
				_uint iOverlap = (m_tAnimTex_Desc.uRowColumn.x + 1) * (m_tAnimTex_Desc.uRowColumn.y + 1) - m_tAnimTex_Desc.iFrameEnd;

				if (m_tAnimTex_Desc.uCurRowColumn.x > m_tAnimTex_Desc.uRowColumn.x - iOverlap)
				{
					m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
				}
			}

			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();

			if (m_tAnimTex_Desc.iFrameNow > m_tAnimTex_Desc.iFrameEnd)
			{
				m_tAnimTex_Desc.iFrameNow = m_tAnimTex_Desc.iFrameStart;
			}
		}
	}
	else
	{
		if (m_tAnimTex_Desc.dwSpeed + m_tAnimTex_Desc.dwTime < GetTickCount64() && m_bEffectOnce)
		{
			++m_tAnimTex_Desc.iFrameNow;
			++m_tAnimTex_Desc.uCurRowColumn.x;

			if (m_tAnimTex_Desc.uCurRowColumn.x > m_tAnimTex_Desc.uRowColumn.x)
			{
				m_tAnimTex_Desc.uCurRowColumn.x = 0;
				++m_tAnimTex_Desc.uCurRowColumn.y;
			}

			m_tAnimTex_Desc.uRowColumn.y = ((m_tAnimTex_Desc.iFrameEnd + m_tAnimTex_Desc.uRowColumn.x) / (m_tAnimTex_Desc.uRowColumn.x + 1)) - 1;

			if (m_tAnimTex_Desc.uCurRowColumn.y == m_tAnimTex_Desc.uRowColumn.y)
			{
				_uint iOverlap = (m_tAnimTex_Desc.uRowColumn.x + 1) * (m_tAnimTex_Desc.uRowColumn.y + 1) - m_tAnimTex_Desc.iFrameEnd;

				if (m_tAnimTex_Desc.uCurRowColumn.x >= m_tAnimTex_Desc.uRowColumn.x - iOverlap)
				{
					m_bEffectOnce = false;
					m_tAnimTex_Desc.uCurRowColumn = { 0, 0 };
					return;
				}
			}

			m_tAnimTex_Desc.dwTime = (DWORD)GetTickCount64();

			if (m_tAnimTex_Desc.iFrameNow > m_tAnimTex_Desc.iFrameEnd)
			{
				m_tAnimTex_Desc.iFrameNow = m_tAnimTex_Desc.iFrameStart;
			}
		}
	}
	
}

HRESULT CEffect_Texture::Add_Component()
{

	///* For.Prototype_Component_Texture_VFX_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_VFX_Effect"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/VFX/VFX_%d.png"), 13))))
	//	return E_FAIL;
	//Prototype_Component_Texture_Pixel_Effect
	///* For.Prototype_Component_Texture_FA01_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FA01_Effect"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/FA01/FA01_%d.png"), 10))))
	//	return E_FAIL;


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_VFX_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimTexture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CEffect_Texture* CEffect_Texture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect_Texture* pInstance = new CEffect_Texture(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Texture::Clone(void* _pArg, _uint _iLevel)
{
	CEffect_Texture* pInstance = new CEffect_Texture(*this);

	if (FAILED(pInstance->Initialize(_pArg, _iLevel)))
	{
		MSG_BOX("Failed to Created : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Texture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
