#include "pch.h"
#include "Right_Window.h"

// Editor
#include "Camera_Free.h"
#include "Mesh_Terrain.h"
#include "ImGui_Manager.h"

// Engine
#include "Picking.h"
#include "Transform.h"
#include "Resource_Manager.h"

// 초기 선택값 설정;
static string g_strSelectedTexture;
static string g_strSelectedMesh;

CRight_Window::CRight_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CWindow(_pDevice, _pContext)
    , m_pImGui_Manager(CImGui_Manager::GetInstance())
{
    Safe_AddRef(m_pImGui_Manager);
}

HRESULT CRight_Window::Initialize()
{



#pragma region TEXTURE
    const vector<string>& vecStrTextureFilePaths = m_pGameInstance->Get_VecStrFilePaths(CResource_Manager::TYPE_TEXTURE);
    for (auto& strFilePath : vecStrTextureFilePaths)
    {
        char  szFileName[MAX_PATH] = "";
        char  szExt[MAX_PATH] = "";
        char* szTextureFile = new char[MAX_PATH];

        size_t iCursor = strFilePath.find_last_of(".");
        string strExtension = strFilePath.substr(iCursor + 1);

        if (strExtension == "png" || strExtension == "PNG")
        {
            _splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szTextureFile, MAX_PATH, szFileName);
            strcat_s(szTextureFile, MAX_PATH, szExt);

            m_vecPNGFileNames.push_back(szTextureFile);
        }
        else if (strExtension == "jpg" || strExtension == "JPG")
        {
            _splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szTextureFile, MAX_PATH, szFileName);
            strcat_s(szTextureFile, MAX_PATH, szExt);

            m_vecJPGFileNames.push_back(szTextureFile);
        }
        else if (strExtension == "bmp" || strExtension == "BMP")
        {
            _splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szTextureFile, MAX_PATH, szFileName);
            strcat_s(szTextureFile, MAX_PATH, szExt);

            m_vecBMPFileNames.push_back(szTextureFile);
        }
        /*else if (strExtension == "dds" || strExtension == "DDS")
        {
            _splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szTextureFile, MAX_PATH, szFileName);
            strcat_s(szTextureFile, MAX_PATH, szExt);

            m_vecDDSFileNames.push_back(szTextureFile);
        }*/
        szTextureFile = nullptr;
        //Safe_Delete_Array(szTextureFile);
    }
#pragma endregion





#pragma region MODEL
    const vector<string>& vecStrFBXFilePaths = m_pGameInstance->Get_VecStrFilePaths(CResource_Manager::TYPE_NONANIM);
    for (auto& strFbxFilePath : vecStrFBXFilePaths)
    {
        char  szFileName[MAX_PATH] = "";
        char  szExt[MAX_PATH] = "";
        char* szTextureFile = new char[MAX_PATH];

        size_t iCursor = strFbxFilePath.find_last_of(".");
        string strExtension = strFbxFilePath.substr(iCursor + 1);

        if (strExtension == "bin" || strExtension == "BIN")
        {
            _splitpath_s(strFbxFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szTextureFile, MAX_PATH, szFileName);
            strcat_s(szTextureFile, MAX_PATH, szExt);

            m_vecBINFileNames.push_back(szTextureFile);
        }
        szTextureFile = nullptr;
        //Safe_Delete_Array(szTextureFile);
    }
#pragma endregion

    

    return S_OK;
}

void CRight_Window::Tick(_float _fTimeDelta)
{ 
    const ImGuiViewport* right_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(right_viewport->Size.x - 300.0f, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300.0f, 600.0f), ImGuiCond_Once);
    ImGuiWindowFlags right_window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    if (ImGui::Begin("Resource"), &m_bResource, right_window_flags)
    {
        Show_Resources_List();
        Save_And_Load();
        ImGui::End();
    }


    /*Picking();
     _float4x4 matWorld = static_cast<CTransform*>(static_cast<CPlayer*>(m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")))->Find_Component(TEXT("Com_Transform")))->Get_WorldMatrix();
         static_cast<CTransform*>(m_pPlayer->Find_Component(TEXT("Com_Transform")))->Get_WorldMatrix();

        m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    ImGui::Text("Mouse In Screen Space : (%.1f, %.1f)", mousePos.x, mousePos.y);
    ImGui::Spacing();
    ImGui::Text("Mouse In World Space: (%.f, %.f, %.f)", m_pPlayer->GetPickLocalPos().x, m_pPlayer->GetPickLocalPos().y, m_pPlayer->GetPickLocalPos().z);
    ImGui::Spacing();
    ImGui::Text("Mouse In Local Space: (%.f, %.f, %.f)", m_pPlayer->GetPickLocalPos().x, m_pPlayer->GetPickLocalPos().y, m_pPlayer->GetPickLocalPos().z);*/

    right_viewport = nullptr;
}

HRESULT CRight_Window::Render()
{
    return S_OK;

}

HRESULT CRight_Window::Show_Resources_List()
{      
    vector<const char*> vecTextureFilesList;
    vecTextureFilesList.insert(vecTextureFilesList.end(), m_vecPNGFileNames.begin(), m_vecPNGFileNames.end());
    vecTextureFilesList.insert(vecTextureFilesList.end(), m_vecJPGFileNames.begin(), m_vecJPGFileNames.end());
    vecTextureFilesList.insert(vecTextureFilesList.end(), m_vecBMPFileNames.begin(), m_vecBMPFileNames.end());

    // TabBar
    ImGui::SeparatorText("Resource");
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Texture"))
        {
            if (ImGui::BeginListBox("Texture Lists", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
            {
                m_bMeshIsSelected = false;

                for (size_t i = 0; i < vecTextureFilesList.size(); i++)
                {
                    _bool bIsSelected = (g_strSelectedTexture == vecTextureFilesList[i]);

                    if (ImGui::Selectable(vecTextureFilesList[i], bIsSelected))
                    {
                        ImGui::SetItemDefaultFocus();
                        g_strSelectedTexture = vecTextureFilesList[i];
                        m_bTextureIsSelected = true;
                    }
                }
                ImGui::EndListBox();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mesh"))
        {
            if (ImGui::BeginListBox("Mesh Lists", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
            {
                m_bTextureIsSelected = false;

                for (size_t i = 0; i < m_vecBINFileNames.size(); i++)
                {
                    _bool bIsSelected = (g_strSelectedMesh == m_vecBINFileNames[i]);

                    if (ImGui::Selectable(m_vecBINFileNames[i], bIsSelected))
                    {
                        ImGui::SetItemDefaultFocus();
                        g_strSelectedMesh = m_vecBINFileNames[i];
                        m_bMeshIsSelected = true;
                    }
                }
                ImGui::EndListBox();
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::Separator();

    if (m_bTextureIsSelected)
    {
        if (FAILED(Show_Image(g_strSelectedTexture)))
            return E_FAIL;       
    }

    if (m_bMeshIsSelected)
    {
        if(FAILED(Edit_Coord(g_strSelectedMesh)))
            return E_FAIL;
    }

    
   
    return S_OK;
}

void CRight_Window::Save_And_Load()
{
    ImGui::SeparatorText("Save & Load");
    ImGui::InputTextWithHint("File Name", "Map_Obj.bin", m_chFileName, IM_ARRAYSIZE(m_chFileName));   

    // 임구이에 세팅해준 이유는, 추후 Level_GamePlay의 Tick에서 Load 기능여부에 따라서 파일을 불러오기 위함
    m_pImGui_Manager->Set_szFileName(m_chFileName);

    if (ImGui::Button("Save File", ImVec2(120, 25)))
    {
        m_pImGui_Manager->Save_File(m_chFileName);
        
    }

    ImGui::SameLine();
    if (ImGui::Button("Load File", ImVec2(120, 25)))
    {
        m_pImGui_Manager->Set_IsClicked(true);

        if (strcmp(m_chFileName, "") != 0)
        {
            // 이친구는 정점 정보만 있는 친구들
            // 정점만 있는 친구들은 저 텍스트 창에 써서 불러오기
            string strFileName = m_chFileName;
        }
        else
        {
            // 이친구들은 모든 정보들을 가지고 있는 친구들
            string strFilePath = Find_FilePath(g_strSelectedMesh, CResource_Manager::TYPE_MESH);
            m_pImGui_Manager->Set_strFilePath(strFilePath);
        }
    }
}


HRESULT CRight_Window::Show_Image(const string& _strFilePath)
{
    static ImVec4 Color_multipler(1, 1, 1, 1);
    string strFilePath = Find_FilePath(_strFilePath, CResource_Manager::TYPE_TEXTURE);
    LOADTEXTURE tTexture = Load_Texture(m_pDevice, strFilePath);

    if (ImGui::TreeNode("Texture Viewer"))
    {
        ImGui::Text("File : %s", _strFilePath.c_str());

        ImGui::Text("Size = %d x %d", tTexture.width, tTexture.height);

        ImGui::ColorEdit4("Color", &Color_multipler.x);

        // 텍스처가 로드되었다면 ImGui에서 사용할 수 있는 텍스처로 변환하여 이미지 표시
        if (nullptr != tTexture.srv)
        {
            ImGui::Separator();
            ImGui::Image((ImTextureID)tTexture.srv, ImVec2(210, 210), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), Color_multipler);
        }
        else
        {
            ImGui::Text("Failed to load texture");
            return E_FAIL;
        }
        ImGui::TreePop();
    }  
   
    return S_OK;
}

string CRight_Window::Find_FilePath(const string& _strFileName, CResource_Manager::TYPE _eType)
{
    string strFilePath;

    const vector<string>& vecStrFilePaths = m_pGameInstance->Get_VecStrFilePaths(_eType);

    for (auto& vecStrFilePath : vecStrFilePaths)
    {
        char  szFileName[MAX_PATH] = "";
        char  szExt[MAX_PATH] = "";
        char* szTextureFile = new _char[MAX_PATH];     
       
        _splitpath_s(vecStrFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

        strcpy_s(szTextureFile, MAX_PATH, szFileName);
        strcat_s(szTextureFile, MAX_PATH, szExt);      

        if (!strcmp(_strFileName.c_str(), szTextureFile))
        {
            strFilePath = vecStrFilePath;
            break;
        }             

        Safe_Delete_Array(szTextureFile);
    }

    return strFilePath;
}

LOADTEXTURE CRight_Window::Load_Texture(ID3D11Device* _pDevice, const string& _strFilePath)
{
    wstring wstrFilePath;
    LOADTEXTURE tTexture;

    wstrFilePath.assign(_strFilePath.begin(), _strFilePath.end());

    DirectX::CreateWICTextureFromFile(_pDevice, wstrFilePath.c_str(), nullptr, &tTexture.srv);

    if (tTexture.srv != nullptr) {
        ID3D11Resource* TextureResource;
        tTexture.srv->GetResource(&TextureResource);

        D3D11_TEXTURE2D_DESC desc;
        ((ID3D11Texture2D*)TextureResource)->GetDesc(&desc);

        tTexture.width = desc.Width;
        tTexture.height = desc.Height;

        // 텍스처 리소스 Release
        TextureResource->Release();
    }

    return tTexture;
}

HRESULT CRight_Window::Edit_Coord(const string& _strFileName)
{

    if (ImGui::TreeNode("Coordinate Editor"))
    {
       // ImGui::Separator();
        ImGui::Text("File : %s", _strFileName.c_str());
        ImGui::Separator();
        // 마우스 위치 얻어오기
        ImGuiIO& io = ImGui::GetIO();

        if (nullptr != m_pImGui_Manager->Get_CurOBJ())
        {
            ImGui::Text("World Pos X: %.1f Y: %.1f Z: %.1f",
                dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix().r[3].m128_f32[0],
                dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix().r[3].m128_f32[1],
                dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix().r[3].m128_f32[2]);

            ImGui::Separator();

            _float4x4 WorldMatrix;
            XMStoreFloat4x4(&WorldMatrix, dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Get_WorldMatrix());
            EditTransform(WorldMatrix);

        }

        ImGui::TreePop();
    }


    return S_OK;
}

void CRight_Window::EditTransform(_float4x4 _WorldMatrix)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::SCALE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

    float vMeshMatrix[16] = {
           _WorldMatrix._11, _WorldMatrix._12, _WorldMatrix._13, _WorldMatrix._14,
           _WorldMatrix._21, _WorldMatrix._22, _WorldMatrix._23, _WorldMatrix._24,
           _WorldMatrix._31, _WorldMatrix._32, _WorldMatrix._33, _WorldMatrix._34,
           _WorldMatrix._41, _WorldMatrix._42, _WorldMatrix._43, _WorldMatrix._44,
    };

    // 마우스 
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    // 입력창
    float vScale[3], vRotation[3], vTranslation[3];
    ImGuizmo::DecomposeMatrixToComponents(vMeshMatrix, vTranslation, vRotation, vScale);
    ImGui::InputFloat3("S", vScale, "%.2f");
    ImGui::InputFloat3("R", vRotation, "%.2f");
    ImGui::InputFloat3("T", vTranslation, "%.2f");
    ImGuizmo::RecomposeMatrixFromComponents(vTranslation, vRotation, vScale, vMeshMatrix);

    // 월드 & 로컬 버튼
    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }


    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW).r->m128_f32, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ).r->m128_f32, mCurrentGizmoOperation, mCurrentGizmoMode, vMeshMatrix, NULL, NULL);


    _fvector Right{ vMeshMatrix[0], vMeshMatrix[1],vMeshMatrix[2], vMeshMatrix[3] };
    _fvector Up{ vMeshMatrix[4], vMeshMatrix[5],vMeshMatrix[6], vMeshMatrix[7] };
    _fvector Look{ vMeshMatrix[8], vMeshMatrix[9],vMeshMatrix[10], vMeshMatrix[11] };
    _fvector Pos{ vMeshMatrix[12], vMeshMatrix[13],vMeshMatrix[14], vMeshMatrix[15] };

    dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_RIGHT, Right);
    dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_UP, Up);
    dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_LOOK, Look);
    dynamic_cast<class CTransform*>(m_pImGui_Manager->Get_CurOBJ()->Get_Component(TEXT("Com_Transform")))->Set_State(CTransform::STATE_POSITION, Pos);
}

CRight_Window* CRight_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRight_Window* pInstance = new CRight_Window(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CRight_Window");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRight_Window::Free()
{
    __super::Free();

    for (auto& vecPNGs : m_vecPNGFileNames)
        Safe_Delete_Array(vecPNGs);

    for (auto& vecDDSs : m_vecDDSFileNames)
        Safe_Delete_Array(vecDDSs);

    for (auto& vecJPGs : m_vecJPGFileNames)
        Safe_Delete_Array(vecJPGs);

    for (auto& vecBINs : m_vecBINFileNames)
        Safe_Delete_Array(vecBINs);

    Safe_Release(m_pImGui_Manager);
}
