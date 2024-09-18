#include "pch.h"

// ImGui
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "ImGui_Manager.h"

// For Binary
#include <fstream>

#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize(ID3D11Device* _pDevice, 
    ID3D11DeviceContext* _pContext, 
    class CGameInstance* _pGameInstance)
{
    // Safe AddRef
    {
        m_pDevice = _pDevice;
        m_pContext = _pContext;
        m_pGameInstance = _pGameInstance;

        Safe_AddRef(m_pDevice);
        Safe_AddRef(m_pContext);
        Safe_AddRef(m_pGameInstance);
    }
    
    // Setup ImGui Context
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        {
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        }

        // Setup Dear ImGui style
        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        if (!ImGui_ImplWin32_Init(g_hWnd))
            return E_FAIL;
        if (!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
            return E_FAIL;

    }
    
	return S_OK;
}


void CImGui_Manager::BeginTick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void CImGui_Manager::Tick(_float _fTimeDelta)
{
}

void CImGui_Manager::EndTick()
{
    //ImGui::EndFrame();
}

void CImGui_Manager::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::Set_OBJ(wstring _wstrKey, CGameObject* _pGameObject, wstring* _wstrKeyValue)
{
    // 중복된 키가 이미 존재하는 경우 처리
    int count = 1;
    wstring uniqueKey = _wstrKey + L"_" + std::to_wstring(count);

    while (m_mapOBJ.find(uniqueKey) != m_mapOBJ.end()) {
        ++count;
        uniqueKey = _wstrKey + L"_" + std::to_wstring(count);
    }

    m_mapOBJ.emplace(uniqueKey, _pGameObject);

    if(_wstrKeyValue != nullptr)
        (*_wstrKeyValue) = uniqueKey;
}

CGameObject* CImGui_Manager::Get_OBJ(wstring _wstrKey)
{
    auto iter = m_mapOBJ.find(_wstrKey);

    if (iter != m_mapOBJ.end())
        return iter->second;
    
    return nullptr;
}

void CImGui_Manager::Load_File()
{
}

void CImGui_Manager::Save_File(const char* _szFileName)
{
    string strFileName = _szFileName;

    wstring wstrFileName;
    wstrFileName.assign(strFileName.begin(), strFileName.end());

    std::ofstream binaryFile(TEXT("../../Client/Bin/Resources/Binary/NonAnim/") + wstrFileName, std::ios::out | std::ios::binary);

    if (binaryFile.is_open())
    {
        // 맵에 몇개의 객체가 저장되어 있는지
        size_t iCount = m_mapOBJ.size();
        binaryFile.write(reinterpret_cast<const char*>(&iCount), sizeof(size_t));

        for (auto& pair : m_mapOBJ)
        {            
            wstring wstrKeyValue = pair.first;
  
            wchar_t wszName[MAX_PATH];
            wcscpy_s(wszName, wstrKeyValue.c_str());
                
            char pSzName[MAX_PATH]{};
            WideCharToMultiByte(CP_ACP, 0, wszName, -1, pSzName, MAX_PATH, 0, 0);
               
            size_t length = strlen(pSzName);

            binaryFile.write(reinterpret_cast<const char*>(&length), sizeof(size_t));
            binaryFile.write(reinterpret_cast<const char*>(&pSzName), length);

            // 해당 객체의 월드행렬
            CGameObject* pObject = pair.second;          
           
            _float4x4 WorldMatrix;
            XMStoreFloat4x4(&WorldMatrix, dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix());
            binaryFile.write(reinterpret_cast<const char*>(&WorldMatrix), sizeof(_float4x4));        
        }
    }

    binaryFile.close();

    MSG_BOX("Succeed to Save File");
}

void CImGui_Manager::Free()
{
    //if (nullptr != m_pGameObject)
    //    Safe_AddRef(m_pGameObject);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    // Shutdown
    {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplDX11_Shutdown();
        ImGui::DestroyContext();
    }   
}
