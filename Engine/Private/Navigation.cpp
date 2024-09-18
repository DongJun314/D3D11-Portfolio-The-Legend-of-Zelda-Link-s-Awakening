#include "Navigation.h"

// Engine
#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

_float4x4 CNavigation::m_WorldMatrix = {};

CNavigation::CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent(_pDevice, _pContext)
{
}

CNavigation::CNavigation(const CNavigation& _rhs)
    : CComponent(_rhs)
    , m_vecCells(_rhs.m_vecCells)
    , m_pShader(_rhs.m_pShader)
{
    for (auto& pCell : m_vecCells)
        Safe_AddRef(pCell);

    Safe_AddRef(m_pShader);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& _strNavigationDataFilePath, _float3* _pPoints)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    //// �ð� ����, �ݽð� ���� ���⼭ ���ϱ�
   /* if(FAILED(Adjust_to_Clockwise(_pPoints)))
        return E_FAIL;*/

    HRESULT hr = _pPoints == nullptr ? Read_NavigationData(_strNavigationDataFilePath) : Read_VerticeData(_pPoints);
    if (FAILED(hr))
        return E_FAIL;    

    if (FAILED(Make_Neighbors()))
        return E_FAIL;

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CNavigation::Initialize(void* _pArg)
{
    // �׺���̼��� �����Ǿ� �߰��ɶ� m_iCurrentIndex�� �ʱ�ȭ ���ش�
    if (nullptr != _pArg)
    {
        m_iCurrentIndex = ((NAVI_DESC*)_pArg)->iStartCellIndex;
    }

    return S_OK;
}


HRESULT CNavigation::Read_NavigationData(const wstring& _strNavigationDataFilePath)
{
    _ulong dwByte = { 0 };

    HANDLE hFile = CreateFile(_strNavigationDataFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    while (true) // ���� ������ �������� ���� �ֱ⿡ �ݺ������� ���� ���� �� �ֵ��� �Ѵ�, 
    {
        _float3 vPoints[CCell::POINT_END];

        ::ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
        if (0 == dwByte)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_vecCells.size());
        if (nullptr == pCell)
            return E_FAIL;

        // ���Ͽ��� ������ ������ �о� ���� �����̳��� ��� �����Ѵ�
        m_vecCells.push_back(pCell);
    }

    ::CloseHandle(hFile);

    return S_OK;
}

HRESULT CNavigation::Read_VerticeData(const _float3* _pPoints)
{

    CCell* pCell = CCell::Create(m_pDevice, m_pContext, _pPoints, (_uint)m_vecCells.size());
    if (nullptr == pCell)
        return E_FAIL;
    
    // ���Ͽ��� ������ ������ �о� ���� �����̳��� ��� �����Ѵ�
    m_vecCells.push_back(pCell);

    return S_OK;
}

void CNavigation::Tick(_fmatrix _TerrainWorldMatrix)
{
    // ���� �׺� �ǽð����� �̵��� ���, ���� ƽ���� ���� ��Ʈ������ �ٲ��ش�
    XMStoreFloat4x4(&m_WorldMatrix, _TerrainWorldMatrix);
}

_bool CNavigation::Is_Move(_fvector _vPosition)
{
    _int iNeighborIndex = { -1 };

    /* ���� �����ϰ� �ִ� �� �ȿ��� ��������. */
    if (true == m_vecCells[m_iCurrentIndex]->Is_In(_vPosition, &iNeighborIndex))
        return true;

    /* ���� ���� ������. */
    else
    {
        if (-1 != iNeighborIndex)
        {
            while (true)
            {
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_vecCells[iNeighborIndex]->Is_In(_vPosition, &iNeighborIndex))
                {
                    m_iCurrentIndex = iNeighborIndex;
                    return true;
                }
            }
        }

        else
            return false;
    }
}

void CNavigation::Clear_Cell()
{
    m_vecCells.pop_back();
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
     _float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);

    _float4x4 WorldMatrix = m_WorldMatrix;
    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);


    if (-1 == m_iCurrentIndex)
    {
        vColor = _float4(0.f, 1.f, 0.f, 1.f);
    }
    else
    {
        WorldMatrix.m[3][1] += 0.15f;
        vColor = _float4(1.f, 0.f, 0.f, 1.f);
    }

    if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(0);

    if (-1 == m_iCurrentIndex)
    {
        // ���� �����̳� �ȿ� �ִ� Cell�� ��ȸ�ϸ鼭 �׸���!
        for (auto& pCell : m_vecCells)
        {
            if (nullptr != pCell)
                pCell->Render();
        }
    }
    else
        m_vecCells[m_iCurrentIndex]->Render();
    
    return S_OK;
}
#endif 

HRESULT CNavigation::Make_Neighbors()
{
    for (auto& pSourCell : m_vecCells)
    {
        for (auto& pDestCell : m_vecCells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
            if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
        }
    }
    
    return S_OK;
}

HRESULT CNavigation::Adjust_to_Clockwise(_float3* _pPoints)
{
    _vector vPointA = XMLoadFloat3(&_pPoints[0]);
    _vector vPointB = XMLoadFloat3(&_pPoints[1]);
    _vector vPointC = XMLoadFloat3(&_pPoints[2]);
    
    _vector vDirAB = XMVectorSubtract(vPointB, vPointA);  // AB ���� ����
    _vector vDirAC = XMVectorSubtract(vPointC, vPointA);  // AC ���� ����

    // �� ������ ���� ���
    _vector vCross = XMVector3Cross(vDirAB, vDirAC);

    _float vCrossY = XMVectorGetY(vCross);

    if (vCrossY < 0)
    {
        XMStoreFloat3(&_pPoints[1], vPointC);
        XMStoreFloat3(&_pPoints[2], vPointB);
    }

    return S_OK;
}

HRESULT CNavigation::Merge_Points(_float3* _pPoints)
{
    for (auto& pSourCell : m_vecCells)
    {
        if (FAILED(Compare_Points_Dis(pSourCell->Get_Point(CCell::POINT_A), _pPoints)))
            return E_FAIL;

        if (FAILED(Compare_Points_Dis(pSourCell->Get_Point(CCell::POINT_B), _pPoints)))
            return E_FAIL;
        
        if (FAILED(Compare_Points_Dis(pSourCell->Get_Point(CCell::POINT_C), _pPoints)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CNavigation::Compare_Points_Dis(const _float3 _pSourPoint, _float3* _pDestPoints)
{  
    // �Ÿ�AA
    _float fDisX = fabs(_pSourPoint.x - _pDestPoints[0].x);
    _float fDisY = fabs(_pSourPoint.y - _pDestPoints[0].y);
    _float fDisZ = fabs(_pSourPoint.z - _pDestPoints[0].z);

    _float fDis = fDisX + fDisY + fDisZ;

    if (fDis < 50.f)
    {
        _pDestPoints[0] = _pSourPoint;
        return S_OK;
    }

    // �Ÿ� AB
    fDisX = fabs(_pSourPoint.x - _pDestPoints[1].x);
    fDisY = fabs(_pSourPoint.y - _pDestPoints[1].y);
    fDisZ = fabs(_pSourPoint.z - _pDestPoints[1].z);

    fDis = fDisX + fDisY + fDisZ;

    if (fDis < 50.f)
    {
        _pDestPoints[1] = _pSourPoint;
        return S_OK;
    }

    // �Ÿ� AC
   fDisX = fabs(_pSourPoint.x - _pDestPoints[2].x);
   fDisY = fabs(_pSourPoint.y - _pDestPoints[2].y);
   fDisZ = fabs(_pSourPoint.z - _pDestPoints[2].z);

   fDis = fDisX + fDisY + fDisZ;

    if (fDis < 50.f)
    {
        _pDestPoints[2] = _pSourPoint;
        return S_OK;
    }

    return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const wstring& _strNavigationDataFilePath, _float3* _pPoints)
{
    CNavigation* pInstance = new CNavigation(_pDevice, _pContext);
    
    if (FAILED(pInstance->Initialize_Prototype(_strNavigationDataFilePath, _pPoints)))
    {
        MSG_BOX("Failed to Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigation::Clone(void* _pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();

    for (auto& pCell : m_vecCells)
        Safe_Release(pCell);
    m_vecCells.clear();


#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif // _DEBUG


}
