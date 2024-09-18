#include "pch.h"
#include "Left_Window.h"

CLeft_Window::CLeft_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CWindow(_pDevice, _pContext)
{
}

HRESULT CLeft_Window::Initialize()
{


    return S_OK;
}

void CLeft_Window::Tick(_float _fTimeDelta)
{
    ImVec2 mousePos = ImGui::GetMousePos();
    const ImGuiViewport* left_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(180, left_viewport->Pos.y + 84), ImGuiCond_Once);
    ImGuiWindowFlags left_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    if (ImGui::Begin("Coordinate"), &m_bMode, left_window_flags)
    {
        // 마우스 위치 얻어오기
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("View  X: %.1f Y: %.1f", io.MousePos.x, io.MousePos.y);
        ImGui::Text("World X: %.1f Y: %.1f", io.MousePos.x, io.MousePos.y);
        ImGui::Text("Local X: %.1f Y: %.1f", io.MousePos.x, io.MousePos.y);


        ImGui::End();
    }

}

HRESULT CLeft_Window::Render()
{
    return S_OK;
}

CLeft_Window* CLeft_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLeft_Window* pInstance = new  CLeft_Window(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLeft_Window");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLeft_Window::Free()
{
    __super::Free();
}
