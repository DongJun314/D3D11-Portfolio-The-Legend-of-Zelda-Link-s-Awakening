#include "stdafx.h"
#include "LandObject.h"

// Engine
#include "Cell.h"

#include "Navigation.h"

CLandObject::CLandObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject(_pDevice, _pContext)
{
}

CLandObject::CLandObject(const CLandObject& _rhs)
    : CGameObject(_rhs)
{
}

HRESULT CLandObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLandObject::Initialize(void* _pArg)
{
    LANDOBJ_DESC* pGameObjectDesc = (LANDOBJ_DESC*)_pArg;
    {
        m_pNavTransform = pGameObjectDesc->pNavTransform;
        m_vecCells = pGameObjectDesc->vecCells;
        Safe_AddRef(m_pNavTransform);
        
        for (size_t i = 0; i < m_vecCells.size(); i++)
            Safe_AddRef(m_vecCells[i]);

        // Mini GameA
        m_pGameA_NavTransform = pGameObjectDesc->pGameA_NavTransform;
        m_vecGameA_Cells = pGameObjectDesc->vecGameA_Cells;
        Safe_AddRef(m_pGameA_NavTransform);

        for (size_t i = 0; i < m_vecGameA_Cells.size(); i++)
            Safe_AddRef(m_vecGameA_Cells[i]);

        // Boss StageB
        m_pBossStage_NavTransform = pGameObjectDesc->pBossStage_NavTransform;
        m_vecBossStage_Cells = pGameObjectDesc->vecBossStage_Cells;

        Safe_AddRef(m_pBossStage_NavTransform);

        for (size_t i = 0; i < m_vecBossStage_Cells.size(); i++)
            Safe_AddRef(m_vecBossStage_Cells[i]);
    }

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CLandObject::Priority_Tick(_float _fTimeDelta)
{
}

void CLandObject::Tick(_float _fTimeDelta)
{
}

void CLandObject::Late_Tick(_float _fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
    return S_OK;
}

HRESULT CLandObject::SetUp_OnTerrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
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

HRESULT CLandObject::SetUp_OnMiniGameA_Terrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
{
    // 높이 맵 적용이 필요한 객체의 Transform 위치를 얻어온다(현재 월드 상태)
    _vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE_POSITION);
    {
        // 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
        vTargetPos = XMVector3TransformCoord(vTargetPos, m_pGameA_NavTransform->Get_WorldMatrix_Inverse());

        // 해당 객체의 Y축 좌표를 계산해서 반환한다 
        _int iCellIndex = { -1 };

        if (_pNavigation->Is_Move(vTargetPos))
        {
            iCellIndex = _pNavigation->Get_CellIndex();
            vTargetPos = XMVectorSetY(vTargetPos, m_vecGameA_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vTargetPos));
        }

        // 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
        _pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pGameA_NavTransform->Get_WorldMatrix()));
    }

    return S_OK;
}

HRESULT CLandObject::SetUp_OnBossStage_Terrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
{   
    // 높이 맵 적용이 필요한 객체의 Transform 위치를 얻어온다(현재 월드 상태)
    _vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE_POSITION);
    {
        // 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
        vTargetPos = XMVector3TransformCoord(vTargetPos, m_pBossStage_NavTransform->Get_WorldMatrix_Inverse());

        // 해당 객체의 Y축 좌표를 계산해서 반환한다 
        _int iCellIndex = { -1 };

        if (_pNavigation->Is_Move(vTargetPos))
        {
            iCellIndex = _pNavigation->Get_CellIndex();
            vTargetPos = XMVectorSetY(vTargetPos, m_vecBossStage_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vTargetPos));
        }

        // 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
        _pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pBossStage_NavTransform->Get_WorldMatrix()));
    }

    return S_OK;
}

HRESULT CLandObject::SetUp_OnFirstView_Terrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
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
        vTargetPos.m128_f32[1] += 0.5f;
        // 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
        _pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pNavTransform->Get_WorldMatrix()));
    }
    return S_OK;
}

HRESULT CLandObject::SetUp_OnFirstView_MiniGameA_Terrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
{
    // 높이 맵 적용이 필요한 객체의 Transform 위치를 얻어온다(현재 월드 상태)
    _vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE_POSITION);
    {
        // 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
        vTargetPos = XMVector3TransformCoord(vTargetPos, m_pGameA_NavTransform->Get_WorldMatrix_Inverse());

        // 해당 객체의 Y축 좌표를 계산해서 반환한다 
        _int iCellIndex = { -1 };

        if (_pNavigation->Is_Move(vTargetPos))
        {
            iCellIndex = _pNavigation->Get_CellIndex();
            vTargetPos = XMVectorSetY(vTargetPos, m_vecGameA_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vTargetPos));
        }
        vTargetPos.m128_f32[1] += 0.8f;
        // 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
        _pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pGameA_NavTransform->Get_WorldMatrix()));
    }

    return S_OK;
}

HRESULT CLandObject::SetUp_OnFirstView_BossStage_Terrain(CTransform* _pTargetTransform, CNavigation* _pNavigation)
{
    // 높이 맵 적용이 필요한 객체의 Transform 위치를 얻어온다(현재 월드 상태)
    _vector vTargetPos = _pTargetTransform->Get_State(CTransform::STATE_POSITION);
    {
        // 해당 객체의 좌표에 월드 역행렬을 곱해주는게 아닌, 지형의 월드로 가야 비교를 할 수 있으니, 지형의 월드 역행렬을 곱해준다. (왜? 로컬좌표에 있는 터레인과 비교를 해야하기 때문에)
        vTargetPos = XMVector3TransformCoord(vTargetPos, m_pBossStage_NavTransform->Get_WorldMatrix_Inverse());

        // 해당 객체의 Y축 좌표를 계산해서 반환한다 
        _int iCellIndex = { -1 };

        if (_pNavigation->Is_Move(vTargetPos))
        {
            iCellIndex = _pNavigation->Get_CellIndex();
            vTargetPos = XMVectorSetY(vTargetPos, m_vecBossStage_Cells[iCellIndex]->Get_VIBuffer_Cell()->Compute_Height(vTargetPos));
        }
        vTargetPos.m128_f32[1] += 1.f;
        // 다시 해당 객체의 좌표에다가 월드행렬을 곱해서, 월드 좌표로 만들고, 해당 객체에 새롭게 세팅해준다
        _pTargetTransform->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(vTargetPos, m_pBossStage_NavTransform->Get_WorldMatrix()));
    }

    return S_OK;
}

void CLandObject::Free()
{
    __super::Free();

    Safe_Release(m_pNavTransform);
    for (size_t i = 0; i < m_vecCells.size(); i++)
        Safe_Release(m_vecCells[i]);


    // Mini GameA
    Safe_Release(m_pGameA_NavTransform);
    for (size_t i = 0; i < m_vecGameA_Cells.size(); i++)
        Safe_Release(m_vecGameA_Cells[i]);


    // Mini GameB
    Safe_Release(m_pBossStage_NavTransform);

    for (size_t i = 0; i < m_vecBossStage_Cells.size(); i++)
        Safe_Release(m_vecBossStage_Cells[i]);
}
