#include "pch.h"
#include "FEM.h"
#include "Text/Gui.h"
#include "Objects/GridLayOut.h"
#include "Integrals.h"
#include "MathFunctions.h"
#include "Rays/Ray3D.h"
#include "Core/Editor.h"

using namespace Doom;

double Doom::Triangle::CalculateAreaOfTriangle()
{
    dvec3 x = dvec3(m_Nodes[0]->m_Pos.x, m_Nodes[1]->m_Pos.x, m_Nodes[2]->m_Pos.x);
    dvec3 y = dvec3(m_Nodes[0]->m_Pos.y, m_Nodes[1]->m_Pos.y, m_Nodes[2]->m_Pos.y);
    return abs(0.5 * (x[0] * (y[1] - y[2]) + x[1] * (y[2] - y[0]) + x[2] * (y[0] - y[1])));
}

double Doom::Triangle::CalculateRatCentroid()
{
    return glm::abs((m_Nodes[0]->m_Pos.x + m_Nodes[1]->m_Pos.x + m_Nodes[2]->m_Pos.x) / 3.0);
}

double Doom::Triangle::CalculateZatCentroid()
{
    return  glm::abs((m_Nodes[0]->m_Pos.y + m_Nodes[1]->m_Pos.y + m_Nodes[2]->m_Pos.y) / 3.0);
}

void Doom::FEM::OnUpdate()
{
    Window::GetInstance().GetCamera().CameraMovement();
    if (m_R3d->m_Mesh != nullptr && m_IsIndexed == false)
    {
        IndexNodes(m_R3d->m_Mesh);
        m_IsIndexed = true;
    }
    ChoseNode();
}

glm::dvec3 Doom::Triangle::CalculateAlpha()
{
    glm::dvec3 a;
    a.x = m_Nodes[1]->m_Pos.x * m_Nodes[2]->m_Pos.y - m_Nodes[2]->m_Pos.x * m_Nodes[1]->m_Pos.y;
    a.y = m_Nodes[2]->m_Pos.x * m_Nodes[0]->m_Pos.y - m_Nodes[0]->m_Pos.x * m_Nodes[2]->m_Pos.y;
    a.z = m_Nodes[0]->m_Pos.x * m_Nodes[1]->m_Pos.y - m_Nodes[1]->m_Pos.x * m_Nodes[0]->m_Pos.y;
    return a;
}

glm::dvec3 Doom::Triangle::CalculateBeta()
{
    glm::dvec3 b;
    b.x = m_Nodes[1]->m_Pos.y - m_Nodes[2]->m_Pos.y;
    b.y = m_Nodes[2]->m_Pos.y - m_Nodes[0]->m_Pos.y;
    b.z = m_Nodes[0]->m_Pos.y - m_Nodes[1]->m_Pos.y;
    return b;
}

void Doom::FEM::Clear()
{
    m_CurrentNode = nullptr;
    m_NodeSphere->m_Enable = false;
    m_MaxStress = math::Vector(4, 0.0);
    m_ProgressMessage = "";
    ClearForces();
    ClearConstraints();
    m_GlobalStiffnessMatrix = math::Matrix(0, 0);

    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        delete g_Nodes[i];
    }
    g_Nodes.clear();
    for (size_t i = 0; i < m_Triangles.size(); i++)
    {
        delete m_Triangles[i];
    }
    for (size_t i = 0; i < 4; i++)
    {
        m_ShowStress[i] = false;
        m_ConstraintsSidesX[i] = false;
        m_ConstraintsSidesY[i] = false;
        m_ForcesValuesX[i] = 0;
        m_ForcesValuesY[i] = 0;
    }
    m_Triangles.clear();
    if(m_IsCalculatedGSM)
        delete[] m_DeformedMesh;
    if (m_R3d->m_Mesh != nullptr)
    {
        m_R3d->m_Mesh->m_VertAttrib = m_DefaultMesh;
        m_DefaultMesh = nullptr;
        MeshManager::GetInstance().DeleteMesh(m_R3d->m_Mesh);
        m_R3d->LoadMesh(nullptr);
    }
    m_MinAndMaxSidePos = { -1e7, 1e7, -1e7, 1e7 };
    m_IsCalculatedGSM = false;
    m_ShowDisplacements = false;
    m_IsIndexed = false;
    m_StaticNodes.clear();
    m_ShowForces = true;
    m_ShowConstraints = true;
    ShowForces();
    ShowConstraints();
}

void Doom::FEM::ClearForces()
{
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        Node* node = g_Nodes[i];
        node->m_Forces.x = 0.0;
        node->m_Forces.y = 0.0;
        if (node->m_ForceArrowX != nullptr) World::GetInstance().DeleteObject(node->m_ForceArrowX->m_Id);
        if (node->m_ForceArrowY != nullptr) World::GetInstance().DeleteObject(node->m_ForceArrowY->m_Id);
        node->m_ForceArrowX = nullptr;
        node->m_ForceArrowY = nullptr;
    }
}

glm::dvec3 Doom::Triangle::CalculateGamma()
{
    glm::dvec3 c;
    c.x = m_Nodes[2]->m_Pos.x - m_Nodes[1]->m_Pos.x;
    c.y = m_Nodes[0]->m_Pos.x - m_Nodes[2]->m_Pos.x;
    c.z = m_Nodes[1]->m_Pos.x - m_Nodes[0]->m_Pos.x;
    return c;
}

math::Matrix Doom::Triangle::CalculateMatrixK(math::Matrix matD, std::vector<Weight>& weights)
{
    math::Matrix elementMatK = math::Matrix(6, 6);
    m_MatB = math::Matrix(4, 6);
    for (size_t k = 0; k < 7; k++)
    {
        Weight weight = weights[k];
        double r = weight.m_L1 * m_Nodes[0]->m_Pos.x + weight.m_L2 * m_Nodes[1]->m_Pos.x + ((1.0 - weight.m_L1 - weight.m_L2) * m_Nodes[2]->m_Pos.x);
        double z = weight.m_L1 * m_Nodes[0]->m_Pos.y + weight.m_L2 * m_Nodes[1]->m_Pos.y + ((1.0 - weight.m_L1 - weight.m_L2) * m_Nodes[2]->m_Pos.y);
        math::Matrix matB = CalculateMatrixB(weight, r, z);
        math::Matrix matK = math::Transpose(matB) * matD * matB;
        m_MatB += matB * weight.m_W * 2.0;
        for (size_t u = 0; u < elementMatK.m_Rows; u++)
        {
            for (size_t j = 0; j < elementMatK.m_Cols; j++)
            {
                elementMatK.operator()(u, j) += matK.operator()(u, j) * r * weight.m_W * m_Area * 2.0 * glm::pi<double>() * 2.0;
            }
        }
    }
    std::cout << "MatrixK\n";
    math::Print(elementMatK);
    return elementMatK;
}

void Doom::FEM::OnClose()
{
    Clear();
}

void Doom::FEM::ChangeColor(int stressType, bool isWhite)
{
    for (size_t i = 0; i < m_Triangles.size(); i++)
    {
        double relativeStress = m_ShowRelativeToStress ? m_RelativeStress : m_MaxStress[stressType];
        glm::vec3 color1 = DoubleToColor(glm::clamp((abs(m_Triangles[i]->m_Nodes[0]->m_Stress[stressType]) / relativeStress), 0.0, 1.0));
        glm::vec3 color2 = DoubleToColor(glm::clamp((abs(m_Triangles[i]->m_Nodes[1]->m_Stress[stressType]) / relativeStress), 0.0, 1.0));
        glm::vec3 color3 = DoubleToColor(glm::clamp((abs(m_Triangles[i]->m_Nodes[2]->m_Stress[stressType]) / relativeStress), 0.0, 1.0));
        //double stress = glm::clamp((abs(m_Triangles[i]->m_Stress[stressType]) / 10e6), 0.0, 1.0); //
        //if (isWhite) color = glm::vec3(1.0f);
        //else color = DoubleToColor(stress);
        m_Triangles[i]->m_XPtr[8] = color1.x;
        m_Triangles[i]->m_XPtr[9] = color1.y;
        m_Triangles[i]->m_XPtr[10] = color1.z;
        m_Triangles[i]->m_XPtr[17 + 8] = color2.x;
        m_Triangles[i]->m_XPtr[17 + 9] = color2.y;
        m_Triangles[i]->m_XPtr[17 + 10] = color2.z;
        m_Triangles[i]->m_XPtr[17 * 2 + 8] = color3.x;
        m_Triangles[i]->m_XPtr[17 * 2 + 9] = color3.y;
        m_Triangles[i]->m_XPtr[17 * 2 + 10] = color3.z;
    }
}

void Doom::FEM::OnGuiRender()
{
    Mesh* mesh = m_R3d->m_Mesh;
    Gui& g = Gui::GetInstance();
    g.m_XAlign = g.LEFT;
    g.RelateToPanel();
    g.m_RelatedPanelProperties.m_AutoAllignment = true;
    g.m_RelatedPanelProperties.m_Margin.y = 25;
    g.m_RelatedPanelProperties.m_Margin.x = 25;
    g.m_RelatedPanelProperties.m_Padding.y = 25;
    g.Panel(L"##1", -875, 0, 400, 1150, glm::vec4(0.3, 0.3, 0.3, 0.8));
    if (mesh == nullptr)
    {
        if (g.Button(L"Choose mesh", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
        {
            std::optional<std::string> filePath = FileDialogs::OpenFile("Meshes (*.fbx)\0");
            if (filePath) {
                std::string name = Utils::GetNameFromFilePath(*filePath);
                MeshManager::GetInstance().AsyncLoadMesh(name, *filePath);
                MeshManager::GetInstance().GetMeshWhenLoaded(name, m_R3d);
            }
        }
    }
    else if (m_IsSolvingSOLE == false && g.Button(L"Reset", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
    {
        Clear();
    }
    if (mesh != nullptr)
    {
        if (m_IsCalculatedGSM == false && m_IsSolvingSOLE == false)
        {
            if (g.CollapsingHeader(L"Forces", 0, 0, COLORS::Gray * 0.8f))
            {
                glm::vec2 tempPadding = g.m_RelatedPanelProperties.m_Padding;
                glm::vec2 tempMargin = g.m_RelatedPanelProperties.m_Margin;
                g.m_RelatedPanelProperties.m_Padding.y = 3;
                g.m_RelatedPanelProperties.m_Margin = { 50, 3};
                if (g.CollapsingHeader(L"#F X", 0, 0, COLORS::Gray * 0.8f)) {
                    for (size_t i = 0; i < 4; i++)
                    {
                        //g.CheckBox("Side: " + std::to_string(i), &m_ForcesSidesX[i], 0, 0, 25);
                        g.InputInt(L"##Forces" + std::to_wstring(i) + L" " + m_SideNames[i], &m_ForcesValuesX[i], 0, 0, 200, 50);
                    }
                }
                if (g.CollapsingHeader(L"#F Y", 0, 0, COLORS::Gray * 0.8f)) {
                    for (size_t i = 0; i < 4; i++)
                    {
                        //g.CheckBox("Side: " + std::to_string(i), &m_ForcesSidesY[i], 0, 0, 25);
                        g.InputInt(L"##Forces" + std::to_wstring(i + 4) + L" " + m_SideNames[i], &m_ForcesValuesY[i], 0, 0, 200, 50);
                    }
                }
                if (g.Button(L"Apply", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
                {
                    ClearForces();
                    SetForcesOnSides();
                }
                g.m_RelatedPanelProperties.m_Padding = tempPadding;
                g.m_RelatedPanelProperties.m_Margin = tempMargin;
            }
            if (g.CollapsingHeader(L"Constraints", 0, 0, COLORS::Gray * 0.8f))
            {
                glm::vec2 tempPadding = g.m_RelatedPanelProperties.m_Padding;
                glm::vec2 tempMargin = g.m_RelatedPanelProperties.m_Margin;
                g.m_RelatedPanelProperties.m_Padding.y = 3;
                g.m_RelatedPanelProperties.m_Margin = { 50, 3 };
                if (g.CollapsingHeader(L"#C X", 0, 0, COLORS::Gray * 0.8f)) {
                    for (size_t i = 0; i < 4; i++)
                    {
                        g.CheckBox(L"##Constraints" + std::to_wstring(i + 8) + L" " + m_SideNames[i], &m_ConstraintsSidesX[i], 0, 0, 25);
                    }
                }
                if (g.CollapsingHeader(L"#C Y", 0, 0, COLORS::Gray * 0.8f)) {
                    for (size_t i = 0; i < 4; i++)
                    {
                        g.CheckBox(L"##Constraints" + std::to_wstring(i + 12) + L" " + m_SideNames[i], &m_ConstraintsSidesY[i], 0, 0, 25);
                    }
                }
                if (g.Button(L"Apply", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
                {
                    ClearConstraints();
                    SetConstraintsOnSides();
                }
                g.m_RelatedPanelProperties.m_Padding = tempPadding;
                g.m_RelatedPanelProperties.m_Margin = tempMargin;
            }
            if (g.Button(L"Calculate GSM", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
            {
                m_NodeSphere->m_Enable = false;
                ThreadPool::GetInstance().Enqueue([=] {
                    m_Progress = 0.0;
                    m_IsSolvingSOLE = true;
                    CalculateGlobalStiffnessMatrix(mesh);
                    m_IsSolvingSOLE = false;
                });
                m_IsCalculatedGSM = true;
            }
        }
        if (g.CheckBox(L"Show forces", &m_ShowForces, 0, 0, 25))
        {
            ShowForces();
        }
        if (g.CheckBox(L"Show constraints", &m_ShowConstraints, 0, 0, 25))
        {
            ShowConstraints();
        }
        g.CheckBox(L"Is wire mesh", &m_R3d->m_IsWireMesh, 0, 0, 25);
        g.CheckBox(L"Show bounding box", &Editor::GetInstance().isBoundingBoxesVisible, 0, 0, 25);
    }
    if (m_IsCalculatedGSM && m_IsSolvingSOLE == false)
    {
        if (g.CheckBox(L"Show displacements", &m_ShowDisplacements, 0, 0, 25))
        {
            if (m_ShowDisplacements)
            {
                mesh->m_VertAttrib = m_DeformedMesh;
            }
            else
            {
                mesh->m_VertAttrib = m_DefaultMesh;
            }
            mesh->Refresh();
        }
        if (g.CollapsingHeader(L"Stresses", 0, 0, COLORS::Gray * 0.8f))
        {
            for (size_t i = 0; i < 4; i++)
            {
                if (g.CheckBox(m_StressesNames[i], &m_ShowStress[i], 0, 0, 25))
                {
                    if (m_ShowStress[i])
                    {
                       
                        ChangeColor(i, false);
                    }
                    else
                    {
                        ChangeColor(i, true);
                    }
                    for (size_t l = 0; l < 4; l++)
                    {
                        if (i != l)
                        {
                            m_ShowStress[l] = false;
                        }
                    }
                    mesh->Refresh();
                }
            }
        }
    }
    if (m_IsCalculatedGSM == false)
    {
        g.InputInt(L"MPa E", &m_InputE, 0, 0, 200, 50);
        m_E = m_InputE * 10e6;
        g.InputInt(L"*0.1 U", &m_InputU, 0, 0, 200, 50);
        m_U = m_InputU * 0.1;
    }

    if (g.CheckBox(L"Relative to stress", &m_ShowRelativeToStress, 0, 0, 25));
    if (m_ShowRelativeToStress)
    {
        m_RelativeStress /= 10e6;
        g.InputInt(L"Mpa", &m_RelativeStress, 0, 0, 200, 50);
        m_RelativeStress *= 10e6;
    }

    if(g.SliderFloat(L"Light angle", &m_DirLightAngle, -180.0f, 180.0f, 0, 0, 175, 30))
    {
        World::GetInstance().s_GameObjects[1]->m_Transform.RotateOnce(m_DirLightAngle, 0, 0);
    }
    if (g.Button(L"Exit", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
    {
        Window::GetInstance().Exit();
    }
    g.m_RelatedPanelProperties.m_AutoAllignment = false;
    g.UnRelateToPanel();
    g.Text(L"FPS %f", true, 800, 450, 40, COLORS::White, 0, Window::GetInstance().GetFPS());
    if (m_IsSolvingSOLE)
    {
        g.Bar(-100.0f, 0.0f, m_Progress, 1.0f, COLORS::Green, COLORS::Gray, 300, 50);
        g.Text(L"%f", true, 220, -13, 20, COLORS::White, 2, m_Progress);
        g.Text(L"%s", true, -100, -70, 20, COLORS::White, 0, m_ProgressMessage);
    }

    if (m_CurrentNode != nullptr && !m_IsCalculatedGSM)
    {
        g.m_XAlign = g.LEFT;
        g.RelateToPanel();
        g.m_RelatedPanelProperties.m_AutoAllignment = true;
        g.m_RelatedPanelProperties.m_Margin.y = 25;
        g.m_RelatedPanelProperties.m_Margin.x = 25;
        g.m_RelatedPanelProperties.m_Padding.y = 25;
        int64_t forceX = m_CurrentNode->m_Forces.x;
        int64_t forceY = m_CurrentNode->m_Forces.y;
        g.Panel(L"##Node stats", 475, 0, 400, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
        g.Text(L"ID %d", true, 0, 0, 20, COLORS::White, 0, m_CurrentNode->m_Index);
        g.InputInt(L"#n Forces x", &forceX, 0, 0, 200, 50);
        g.InputInt(L"#n Forces Y", &forceY, 0, 0, 200, 50);
        m_CurrentNode->m_Forces = glm::dvec2(forceX, forceY);
        if (g.Button(L"#n Apply", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
        {
            CubeCollider3D* cc = m_Go->GetComponent<CubeCollider3D>();
            glm::vec3 scale = ((cc->m_MaxP - cc->m_MinP) * 0.5f) * 0.2f;
            scale.z = 0.1f;
            if (!math::IsCloseEnough(m_CurrentNode->m_Forces.x, 0.0)) 
            {
                if (m_CurrentNode->m_ForceArrowX != nullptr)
                {
                    World::GetInstance().DeleteObject(m_CurrentNode->m_ForceArrowX->m_Id);
                }
                glm::vec3 pos = glm::vec3(m_CurrentNode->m_Pos.x, m_CurrentNode->m_Pos.y, 0.0);
                glm::vec3 rotation = m_CurrentNode->m_Forces.x > 0 ? glm::vec3(0.0f, 0.0f, -90.0f) : glm::vec3(0.0f, 0.0f, 90.0f);
                m_CurrentNode->m_ForceArrowX = CreateForceArrow(pos, scale, rotation);
            }
            else if (m_CurrentNode->m_ForceArrowX != nullptr)
            {
                World::GetInstance().DeleteObject(m_CurrentNode->m_ForceArrowX->m_Id);
                m_CurrentNode->m_ForceArrowX = nullptr;
            }
            if (!math::IsCloseEnough(m_CurrentNode->m_Forces.y, 0.0))
            {
                if (m_CurrentNode->m_ForceArrowY != nullptr)
                {
                    World::GetInstance().DeleteObject(m_CurrentNode->m_ForceArrowY->m_Id);
                }
                glm::vec3 pos = glm::vec3(m_CurrentNode->m_Pos.x, m_CurrentNode->m_Pos.y, 0.0);
                glm::vec3 rotation = m_CurrentNode->m_Forces.y > 0 ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 180.0f);
                m_CurrentNode->m_ForceArrowY = CreateForceArrow(pos, scale, rotation);
            }
            else if (m_CurrentNode->m_ForceArrowY != nullptr)
            {
                World::GetInstance().DeleteObject(m_CurrentNode->m_ForceArrowY->m_Id);
                m_CurrentNode->m_ForceArrowY = nullptr;
            }
        }
        if (g.CheckBox(L"#n Constraints x", &m_CurrentNode->m_Constraints.x, 0, 0, 25));
        if (g.CheckBox(L"#n Constraints y", &m_CurrentNode->m_Constraints.y, 0, 0, 25));
        if (g.Button(L"#n Apply", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
        {
            CubeCollider3D* cc = m_Go->GetComponent<CubeCollider3D>();
            glm::vec3 scale = ((cc->m_MaxP - cc->m_MinP) * 0.5f) * 0.03f;
            if (m_CurrentNode->m_Constraints.x)
            {
                if (m_CurrentNode->m_ConstraintShpereX == nullptr)
                {
                    glm::vec3 pos = glm::vec3(m_CurrentNode->m_Pos.x, m_CurrentNode->m_Pos.y, 0.0);
                    m_CurrentNode->m_ConstraintShpereX = CreateConstraintsSphere(pos, scale);
                }
                m_StaticNodes.push_back(m_CurrentNode->m_Index * 2 + 0);
            }
            else if (m_CurrentNode->m_ConstraintShpereX != nullptr)
            {
                World::GetInstance().DeleteObject(m_CurrentNode->m_ConstraintShpereX->m_Id);
                auto iter = std::find(m_StaticNodes.begin(), m_StaticNodes.end(), m_CurrentNode->m_Index * 2 + 0);
                if (iter != m_StaticNodes.end()) m_StaticNodes.erase(iter);
                m_CurrentNode->m_ConstraintShpereX = nullptr;
            }
            if (m_CurrentNode->m_Constraints.y)
            {
                if (m_CurrentNode->m_ConstraintShpereY == nullptr)
                {
                    glm::vec3 pos = glm::vec3(m_CurrentNode->m_Pos.x, m_CurrentNode->m_Pos.y, 0.0);
                    m_CurrentNode->m_ConstraintShpereY = CreateConstraintsSphere(pos, scale);
                }
                m_StaticNodes.push_back(m_CurrentNode->m_Index * 2 + 1);
            }
            else if(m_CurrentNode->m_ConstraintShpereY != nullptr)
            {
                World::GetInstance().DeleteObject(m_CurrentNode->m_ConstraintShpereY->m_Id);
                auto iter = std::find(m_StaticNodes.begin(), m_StaticNodes.end(), m_CurrentNode->m_Index * 2 + 1);
                if (iter != m_StaticNodes.end()) m_StaticNodes.erase(iter);
                m_CurrentNode->m_ConstraintShpereY = nullptr;
            }
        }
        if (g.Button(L"#n Close", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
        {
            m_CurrentNode = nullptr;
            m_NodeSphere->m_Enable = false;
        }
        g.m_RelatedPanelProperties.m_AutoAllignment = false;
        g.UnRelateToPanel();
    }
}

void Doom::FEM::AddMatrices(math::Matrix& globalMatK, Triangle* triangle)
{
    std::vector<size_t> localNodes(6);
    localNodes[0] = (triangle->m_Nodes[0]->m_Index * 2) + 0;
    localNodes[1] = (triangle->m_Nodes[0]->m_Index * 2) + 1;
    localNodes[2] = (triangle->m_Nodes[1]->m_Index * 2) + 0;
    localNodes[3] = (triangle->m_Nodes[1]->m_Index * 2) + 1;
    localNodes[4] = (triangle->m_Nodes[2]->m_Index * 2) + 0;
    localNodes[5] = (triangle->m_Nodes[2]->m_Index * 2) + 1;
    for (size_t i = 0; i < triangle->m_MatK.m_Rows; i++)
    {
        for (size_t j = 0; j < triangle->m_MatK.m_Cols; j++)
        {
            globalMatK.operator()(localNodes[i], localNodes[j]) += triangle->m_MatK.operator()(i, j);
        }
    }
}

math::Matrix Doom::FEM::CalculateMatrixD(double E, double u) {
    math::Matrix D(4, 4);
    math::Vector col(4);

    //1
    col[0] = 1.0 - u;
    col[1] = u;
    col[2] = u;
    col[3] = 0.0;
    D.AssignCol(col, 0);

    //2
    col[0] = u;
    col[1] = 1.0 - u;
    col[2] = u;
    col[3] = 0.0;
    D.AssignCol(col, 1);

    //3
    col[0] = u;
    col[1] = u;
    col[2] = 1.0 - u;
    col[3] = 0.0;
    D.AssignCol(col, 2);

    //4
    col[0] = 0.0;
    col[1] = 0.0;
    col[2] = 0.0;
    col[3] = (1.0 - 2.0 * u) * 0.5;
    D.AssignCol(col, 3);
   
    return D * (E / ((1.0 + u) * (1.0 - 2.0 * u)));
}

Doom::Triangle::Triangle(Node* a, Node* b, Node* c)
{
    m_Nodes.push_back(a);
    m_Nodes.push_back(b);
    m_Nodes.push_back(c);
    m_A = CalculateAlpha();
    m_B = CalculateBeta();
    m_Y = CalculateGamma();
    m_Area = CalculateAreaOfTriangle();
}

math::Matrix Doom::Triangle::CalculateMatrixB(Weight weight, double r, double z)
{
    math::Matrix matB(4, 6);
    math::Vector col(4);

    //1
    col[0] = m_B[0];
    col[1] = 0;
    col[2] = (weight.m_L1 / r) * (2.0 * m_Area);
    col[3] = m_Y[0];
    matB.AssignCol(col, 0);

    //2
    col[0] = 0;
    col[1] = m_Y[0];
    col[2] = 0;
    col[3] = m_B[0];
    matB.AssignCol(col, 1);

    //3
    col[0] = m_B[1];
    col[1] = 0.0;
    col[2] = (weight.m_L2 / r) * (2.0 * m_Area);
    col[3] = m_Y[1];
    matB.AssignCol(col, 2);

    //4
    col[0] = 0.0;
    col[1] = m_Y[1];
    col[2] = 0.0;
    col[3] = m_B[1];
    matB.AssignCol(col, 3);
    
    //5
    col[0] = m_B[2];
    col[1] = 0.0;
    col[2] = ((1 - weight.m_L1 - weight.m_L2) / r) * (2.0 * m_Area);
    col[3] = m_Y[2];
    matB.AssignCol(col, 4);

    //6
    col[0] = 0.0;
    col[1] = m_Y[2];
    col[2] = 0.0;
    col[3] = m_B[2];
    matB.AssignCol(col, 5);

    return matB * (1.0 / (2.0 * m_Area));
}

math::Matrix Doom::Triangle::CalculateMatrixBAproximation(double R, double Z)
{
    math::Matrix matB(4,6);
    math::Vector col(4);

    //1
    col[0] = m_B.x;
    col[1] = 0;
    col[2] = m_A.x / R + m_B.x + (m_Y.x * Z) / R;
    col[3] = m_Y.x;
    matB.AssignCol(col, 0);

    //2
    col[0] = 0;
    col[1] = m_Y.x;
    col[2] = 0;
    col[3] = m_B.x;
    matB.AssignCol(col, 1);

    //3
    col[0] = m_B.y;
    col[1] = 0;
    col[2] = m_A.y / R + m_B.y + (m_Y.y * Z) / R;
    col[3] = m_Y.y;
    matB.AssignCol(col, 2);

    //4
    col[0] = 0;
    col[1] = m_Y.y;
    col[2] = 0;
    col[3] = m_B.y;
    matB.AssignCol(col, 3);

    //5
    col[0] = m_B.z;
    col[1] = 0;
    col[2] = m_A.z / R + m_B.z + (m_Y.z * Z) / R;
    col[3] = m_Y.z;
    matB.AssignCol(col, 4);

    //6
    col[0] = 0;
    col[1] = m_Y.z;
    col[2] = 0;
    col[3] =  m_B.z;
    matB.AssignCol(col, 5);

    return matB * (1.0 / (2.0 * m_Area));
}

bool Doom::FEM::CompareNodes(glm::vec2 a, glm::vec2 b)
{
    return (fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON);
}

void Doom::FEM::IndexNodes(Mesh* mesh)
{
    size_t nodeIndex = 0;
    m_DefaultMesh = mesh->m_VertAttrib;
    m_DeformedMesh = new float[mesh->m_VertAttribSize];
    memcpy(m_DeformedMesh, m_DefaultMesh, mesh->m_VertAttribSize * 4);
    mesh->m_VertAttrib = m_DeformedMesh;
    for (uint32_t i = 0; i < mesh->m_VertAttribSize; i += (17 * 3))
    {
        std::vector<glm::vec2> vertices(3);
       vertices[0] = glm::dvec2(mesh->m_VertAttrib[i + 0], mesh->m_VertAttrib[i + 1]);
       vertices[1] = glm::dvec2(mesh->m_VertAttrib[i + 17 + 0], mesh->m_VertAttrib[i + 17 + 1]);
       vertices[2] = glm::dvec2(mesh->m_VertAttrib[i + 17 * 2 + 0], mesh->m_VertAttrib[i + 17 * 2 + 1]);

        mesh->m_VertAttrib[i + 8] = 0.0;
        mesh->m_VertAttrib[i + 9] = 0.0;
        mesh->m_VertAttrib[i + 10] = 0.0;
        mesh->m_VertAttrib[i + 17 + 8] = 0.0;
        mesh->m_VertAttrib[i + 17 + 9] = 0.0;
        mesh->m_VertAttrib[i + 17 + 10] = 0.0;
        mesh->m_VertAttrib[i + 17 * 2 + 8] = 0.0;
        mesh->m_VertAttrib[i + 17 * 2 + 9] = 0.0;
        mesh->m_VertAttrib[i + 17 * 2 + 10] = 0.0;

        std::vector<Node*> tempNodes(3);

        std::cout << "Triangles verteces\n";
        for (size_t i = 0; i < 3; i++)
        {
            auto iterA = std::find_if(g_Nodes.begin(), g_Nodes.end(), [=](Node* node) { return CompareNodes(node->m_Pos, vertices[i]); });
            if (iterA != g_Nodes.end())
            {
                tempNodes[i] = *iterA;
            }
            else
            {
                tempNodes[i] = new Node();
                tempNodes[i]->m_Index = (nodeIndex++);
                g_Nodes.push_back(tempNodes[i]);
                g_Nodes.back()->m_Pos = vertices[i];
            }
            std::cout << tempNodes[i]->m_Index << std::endl;
        }



        m_Triangles.push_back(new Triangle(tempNodes[0], tempNodes[1], tempNodes[2]));
        m_Triangles.back()->m_XPtr = &(mesh->m_VertAttrib[i]);

    }
    //double maxX;
    //double minX;
    //double maxY;
    //double minY;
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        if (m_MinAndMaxSidePos[0] < g_Nodes[i]->m_Pos.x)
        {
            m_MinAndMaxSidePos[0] = g_Nodes[i]->m_Pos.x;
        }
        if (m_MinAndMaxSidePos[3] > g_Nodes[i]->m_Pos.y)
        {
            m_MinAndMaxSidePos[3] = g_Nodes[i]->m_Pos.y;
        }
        if (m_MinAndMaxSidePos[2] < g_Nodes[i]->m_Pos.y)
        {
            m_MinAndMaxSidePos[2] = g_Nodes[i]->m_Pos.y;
        }
        if (m_MinAndMaxSidePos[1] > g_Nodes[i]->m_Pos.x)
        {
            m_MinAndMaxSidePos[1] = g_Nodes[i]->m_Pos.x;
        }
    }
}

void Doom::FEM::OnStart()
{
    ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
   
    Window::GetInstance().GetCamera().SetPosition(glm::vec3(0, 0, 10));
    m_Go = GameObject::Create("Mesh");
    m_R3d = m_Go->AddComponent<Renderer3D>();
    m_R3d->m_IsCullingFace = false;
    GameObject* sun = GameObject::Create("Sun");
    sun->m_Transform.RotateOnce(75, 0, 0);
    sun->AddComponent<DirectionalLight>();
    
    m_NodeSphere = GameObject::Create("NodeSphere");
    m_NodeSphere->m_Enable = false;
    Renderer3D* r3d = m_NodeSphere->AddComponent<Renderer3D>();
    r3d->LoadMesh(MeshManager::GetInstance().GetMesh("sphere"));
    m_NodeSphere->RemoveComponent<CubeCollider3D>();
    r3d->m_Color = COLORS::Yellow;

    //axis
    (new Line(glm::vec3(0.0f, -10000.0f, 0.0f), glm::vec3(0.0f, 10000.0f, 0.0f)))->m_Color = COLORS::Green;
    (new Line(glm::vec3(-10000.0f, 0.0f, 0.0f), glm::vec3(10000.0f, 0.0f, 0.0f)))->m_Color = COLORS::Red;
    (new Line(glm::vec3(0.0f, 0.0f, -10000.0f), glm::vec3(0.0f, 0.0f, 10000.0f)))->m_Color = COLORS::Blue;

    {
        m_Weights.push_back({ 1.0 / 3.0, 1.0 / 3.0, 0.11250 });
        m_Weights.push_back({ 0.0597159, 0.470142,  0.0661971 });
        m_Weights.push_back({ 0.470142,  0.470142,  0.0661971 });
        m_Weights.push_back({ 0.470142,  0.0597159, 0.0661971 });
        m_Weights.push_back({ 0.101287,  0.101287,  0.0629696 });
        m_Weights.push_back({ 0.797427,  0.101287,  0.0629696 });
        m_Weights.push_back({ 0.101287,  0.797427,  0.0629696 });
    }

    m_StressesNames.resize(4);
    m_StressesNames[0] = L"Qr";
    m_StressesNames[1] = L"Qz";
    m_StressesNames[2] = L"Trz";
    m_StressesNames[3] = L"Qo";

    m_SideNames.resize(4);
    m_SideNames[0] = L"Right";
    m_SideNames[1] = L"Left";
    m_SideNames[2] = L"Top";
    m_SideNames[3] = L"Bottom";
}

GameObject* Doom::FEM::CreateForceArrow(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    GameObject* force = GameObject::Create();
    force->m_Transform.RotateOnce(rotation);
    force->m_Transform.Translate(position);
    Renderer3D* r3d = force->AddComponent<Renderer3D>();
    r3d->LoadMesh(MeshManager::GetInstance().GetMesh("Arrow"));
    r3d->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
    r3d->m_Color = COLORS::Blue;
    force->RemoveComponent<CubeCollider3D>();
    force->m_Transform.Scale(scale);
    return force;
}

GameObject* Doom::FEM::CreateConstraintsSphere(glm::vec3 position, glm::vec3 scale)
{
    GameObject* sphere = GameObject::Create("Constraints");
    Renderer3D* r3d = sphere->AddComponent<Renderer3D>();
    r3d->LoadMesh(MeshManager::GetInstance().GetMesh("LowPolySphere"));
    r3d->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
    r3d->m_Color = COLORS::Green;
    sphere->RemoveComponent<CubeCollider3D>();
    sphere->m_Transform.Scale(scale);
    sphere->m_Transform.Translate(position);
    return sphere;
}

void Doom::FEM::ChoseNode()
{
    if (!m_IsCalculatedGSM && !Gui::GetInstance().m_IsAnyPanelHovered && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
    {
        Camera& camera = Window::GetInstance().GetCamera();
        glm::vec3 start = camera.GetPosition();
        glm::vec3 dir = camera.GetMouseDirVec();
        for (size_t i = 0; i < m_Triangles.size(); i++)
        {
            Ray3D::Hit hit;
            glm::dvec3 a = glm::dvec3(m_Triangles[i]->m_Nodes[0]->m_Pos, 0.0);
            glm::dvec3 b = glm::dvec3(m_Triangles[i]->m_Nodes[1]->m_Pos, 0.0);
            glm::dvec3 c = glm::dvec3(m_Triangles[i]->m_Nodes[2]->m_Pos, 0.0);
            glm::dvec3 n = glm::dvec3(0.0, 0.0, 1.0);
            if (Ray3D::IntersectTriangle(start, dir, &hit, 100000.0f, a, b, c, n))
            {
                CubeCollider3D* cc = m_Go->GetComponent<CubeCollider3D>();
                glm::vec3 scale = ((cc->m_MaxP - cc->m_MinP) * 0.5f) * 0.01f;
                scale.z = scale.x;
                m_NodeSphere->m_Transform.Scale(scale);
                m_NodeSphere->m_Enable = true;
                std::map<double, size_t> distances;
                distances.insert(std::make_pair(glm::sqrt(pow(a.x - hit.m_Point.x, 2) * pow(a.y - hit.m_Point.y, 2)), 0));
                distances.insert(std::make_pair(glm::sqrt(pow(b.x - hit.m_Point.x, 2) * pow(b.y - hit.m_Point.y, 2)), 1));
                distances.insert(std::make_pair(glm::sqrt(pow(c.x - hit.m_Point.x, 2) * pow(c.y - hit.m_Point.y, 2)), 2));
                m_CurrentNode = m_Triangles[i]->m_Nodes[distances.begin()->second];
                m_NodeSphere->m_Transform.Translate(glm::vec3(m_CurrentNode->m_Pos, 0.0f));
                return;
            }
        }
    }
}

void Doom::FEM::SetForcesOnSides()
{
    std::vector<std::vector<Node*>> forcesPerSideX(4);
    std::vector<std::vector<Node*>> forcesPerSideY(4);

    CubeCollider3D* cc = m_Go->GetComponent<CubeCollider3D>();
    glm::vec3 scale = ((cc->m_MaxP - cc->m_MinP) * 0.5f) * 0.2f;
    scale.z = 0.1f;
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        g_Nodes[i]->m_Forces.x = 0.0;
        g_Nodes[i]->m_Forces.y = 0.0;

        for (size_t j = 0; j < 2; j++)
        {
            if (m_ForcesValuesX[j] != 0 && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.x))
            {
                forcesPerSideX[j].push_back(g_Nodes[i]);
                //g_Nodes[i]->m_Forces.x = m_ForcesValuesX[j];
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                glm::vec3 rotation = m_ForcesValuesX[j] > 0 ? glm::vec3(0.0f, 0.0f, -90.0f) : glm::vec3(0.0f, 0.0f, 90.0f);
                g_Nodes[i]->m_ForceArrowX = CreateForceArrow(pos, scale, rotation);
            }
        }
        for (size_t j = 2; j < 4; j++)
        {
            if (m_ForcesValuesX[j] != 0 && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.y))
            {
                forcesPerSideX[j].push_back(g_Nodes[i]);
                //g_Nodes[i]->m_Forces.x = m_ForcesValuesX[j];
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                glm::vec3 rotation = m_ForcesValuesX[j] > 0 ? glm::vec3(0.0f, 0.0f, -90.0f) : glm::vec3(0.0f, 0.0f, 90.0f);
                g_Nodes[i]->m_ForceArrowX = CreateForceArrow(pos, scale, rotation);
            }
        }

        for (size_t j = 0; j < 2; j++)
        {
            if (m_ForcesValuesY[j] != 0 && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.x))
            {
                forcesPerSideY[j].push_back(g_Nodes[i]);
                //g_Nodes[i]->m_Forces.y = m_ForcesValuesY[j];
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                glm::vec3 rotation = m_ForcesValuesY[j] > 0 ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 180.0f);
                g_Nodes[i]->m_ForceArrowY = CreateForceArrow(pos, scale, rotation);
            }
        }
        for (size_t j = 2; j < 4; j++)
        {
            if (m_ForcesValuesY[j] != 0 && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.y))
            {
                forcesPerSideY[j].push_back(g_Nodes[i]);
                //g_Nodes[i]->m_Forces.y = m_ForcesValuesY[j];
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                glm::vec3 rotation = m_ForcesValuesY[j] > 0 ? glm::vec3(0.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 180.0f);
                g_Nodes[i]->m_ForceArrowY = CreateForceArrow(pos, scale, rotation);
            }
        }
    }
    for (size_t i = 0; i < 4; i++)
    {
        size_t size = forcesPerSideX[i].size();
        if (size > 0)
        {
            double strength = m_ForcesValuesX[i] / (double)size;
            std::sort(forcesPerSideX[i].begin(), forcesPerSideX[i].end(), [](Node* a, Node* b) {
                if (math::IsCloseEnough(a->m_Pos.x, b->m_Pos.x))
                {
                    return a->m_Pos.y < b->m_Pos.y;
                }
                else if (math::IsCloseEnough(a->m_Pos.y, b->m_Pos.y))
                {
                    return a->m_Pos.x < b->m_Pos.x;
                }
                });

            forcesPerSideX[i][0]->m_Forces.x = strength * 0.5;
            forcesPerSideX[i][size - 1]->m_Forces.x = strength * 0.5;
            for (size_t j = 1; j < size - 1; j++)
            {
                forcesPerSideX[i][j]->m_Forces.x = strength;
            }
        }

        size = forcesPerSideY[i].size();
        if (size > 0)
        {
            double strength = m_ForcesValuesY[i] / (double)size;
            std::sort(forcesPerSideY[i].begin(), forcesPerSideY[i].end(), [](Node* a, Node* b) {
                if (math::IsCloseEnough(a->m_Pos.x, b->m_Pos.x))
                {
                    return a->m_Pos.y < b->m_Pos.y;
                }
                else if (math::IsCloseEnough(a->m_Pos.y, b->m_Pos.y))
                {
                    return a->m_Pos.x < b->m_Pos.x;
                }
                });
            forcesPerSideY[i][0]->m_Forces.y = strength * 0.5;
            forcesPerSideY[i][size - 1]->m_Forces.y = strength * 0.5;
            for (size_t j = 1; j < size - 1; j++)
            {
                forcesPerSideY[i][j]->m_Forces.y = strength;
            }
        }
    }
}

void Doom::FEM::ClearConstraints()
{
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        Node* node = g_Nodes[i];
        node->m_Constraints = glm::bvec2(false);
        if (node->m_ConstraintShpereX != nullptr) World::GetInstance().DeleteObject(node->m_ConstraintShpereX->m_Id);
        if (node->m_ConstraintShpereY != nullptr) World::GetInstance().DeleteObject(node->m_ConstraintShpereY->m_Id);
        node->m_ConstraintShpereX = nullptr;
        node->m_ConstraintShpereY = nullptr;
    }
}

void Doom::FEM::SetConstraintsOnSides()
{
    m_StaticNodes.clear();
    CubeCollider3D* cc = m_Go->GetComponent<CubeCollider3D>();
    glm::vec3 scale = ((cc->m_MaxP - cc->m_MinP) * 0.5f) * 0.03f;
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            if (m_ConstraintsSidesX[j] == true && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.x))
            {
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                g_Nodes[i]->m_ConstraintShpereX = CreateConstraintsSphere(pos, scale);
                m_StaticNodes.push_back(g_Nodes[i]->m_Index * 2 + 0);
                g_Nodes[i]->m_Constraints.x = true;
            }
        }
        for (size_t j = 2; j < 4; j++)
        {
            if (m_ConstraintsSidesX[j] == true && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.y))
            {
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                g_Nodes[i]->m_ConstraintShpereX = CreateConstraintsSphere(pos, scale);
                m_StaticNodes.push_back(g_Nodes[i]->m_Index * 2 + 0);
                g_Nodes[i]->m_Constraints.x = true;
            }
        }

        for (size_t j = 0; j < 2; j++)
        {
            if (m_ConstraintsSidesY[j] == true && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.x))
            {
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                g_Nodes[i]->m_ConstraintShpereY = CreateConstraintsSphere(pos, scale);
                m_StaticNodes.push_back(g_Nodes[i]->m_Index * 2 + 1);
                g_Nodes[i]->m_Constraints.y = true;
            }
        }
        for (size_t j = 2; j < 4; j++)
        {
            if (m_ConstraintsSidesY[j] == true && math::IsCloseEnough(m_MinAndMaxSidePos[j], g_Nodes[i]->m_Pos.y))
            {
                glm::vec3 pos = glm::vec3(g_Nodes[i]->m_Pos.x, g_Nodes[i]->m_Pos.y, 0.0);
                g_Nodes[i]->m_ConstraintShpereY = CreateConstraintsSphere(pos, scale);
                m_StaticNodes.push_back(g_Nodes[i]->m_Index * 2 + 1);
                g_Nodes[i]->m_Constraints.y = true;
            }
        }
    }
}

glm::vec3 Doom::FEM::DoubleToColor(double value)
{
    double hue = (1.0 - value) * 100.0 * 1.2 / 180.0;
    return HslToRGB(hue, 1.0, .5);
}

glm::vec3 Doom::FEM::HslToRGB(double h, double s, double l)
{
    double r, g, b;

    if (math::IsCloseEnough(s, 0.0)) {
        r = g = b = l;
    }
    else {
        auto hue2rgb = [](double p, double q, double t) {
            if (t < 0.0) t += 1.0;
            if (t > 1.0) t -= 1.0;
            if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
            if (t < 1.0 / 2.0) return q;
            if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
            return p;
        };

        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        r = hue2rgb(p, q, h + 1.0 / 3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0 / 3.0);
    }

    return glm::vec3(r, g, b);
}

void Doom::FEM::ShowForces()
{
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        Node* node = g_Nodes[i];
        std::vector<GameObject*> forcesArrows;
        if (node->m_ForceArrowX != nullptr) forcesArrows.push_back(node->m_ForceArrowX);
        if (node->m_ForceArrowY != nullptr) forcesArrows.push_back(node->m_ForceArrowY);
        for (size_t j = 0; j < forcesArrows.size(); j++)
        {
            Renderer3D* r3d = forcesArrows[j]->GetComponent<Renderer3D>();
            if (m_ShowForces)
            {
                r3d->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
                r3d->m_IsRenderable = true;
            }
            else
            {
                r3d->EraseFromInstancing();
                r3d->m_IsRenderable = false;
            }
        }
    }
}

void Doom::FEM::ShowConstraints()
{
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        Node* node = g_Nodes[i];
        std::vector<GameObject*> constraintsSpheres;
        if (node->m_ConstraintShpereX != nullptr) constraintsSpheres.push_back(node->m_ConstraintShpereX);
        if (node->m_ConstraintShpereY != nullptr) constraintsSpheres.push_back(node->m_ConstraintShpereY);
        for (size_t j = 0; j < constraintsSpheres.size(); j++)
        {
            Renderer3D* r3d = constraintsSpheres[j]->GetComponent<Renderer3D>();
            if (m_ShowConstraints)
            {
                r3d->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
                r3d->m_IsRenderable = true;
            }
            else
            {
                r3d->EraseFromInstancing();
                r3d->m_IsRenderable = false;
            }
        }
    }
}

void Doom::FEM::CalculateGlobalStiffnessMatrix(Mesh* mesh)
{
    m_GlobalStiffnessMatrix = math::Matrix(g_Nodes.size() * 2, g_Nodes.size() * 2);
    math::Matrix matD = CalculateMatrixD(m_E, m_U);

    m_ProgressMessage = "Computing global stiffness matrix";
    m_Progress = 0.0;
    for (size_t i = 0; i < m_Triangles.size(); i++)
    {
        m_Triangles[i]->m_MatK = m_Triangles[i]->CalculateMatrixK(matD, m_Weights);
        AddMatrices(m_GlobalStiffnessMatrix, m_Triangles[i]);
        m_Progress = (double)i / (double)m_Triangles.size();
    }
   
    math::Vector forces(m_GlobalStiffnessMatrix.m_Rows);
    m_ProgressMessage = "Assigning forces";
    m_Progress = 0.0;
    for (size_t i = 0; i < g_Nodes.size(); i++)
    {
        forces[g_Nodes[i]->m_Index * 2 + 0] = g_Nodes[i]->m_Forces.x;
        forces[g_Nodes[i]->m_Index * 2 + 1] = g_Nodes[i]->m_Forces.y;
        m_Progress = (double)i / (double)g_Nodes.size();
    }

    std::cout << "Forces\n";
    math::Print(forces);

    size_t removed = 0;
    m_ProgressMessage = "Assigning constraints";
    m_Progress = 0.0;
    for (size_t i = 0; i < m_StaticNodes.size(); i++)
    {
        m_GlobalStiffnessMatrix.RemoveCol(m_StaticNodes[i] - removed);
        m_GlobalStiffnessMatrix.RemoveRow(m_StaticNodes[i] - removed);
        forces.Erase(m_StaticNodes[i] - removed);
        removed++;
        m_Progress = (double)i / (double)m_StaticNodes.size();
    }

    std::cout << "Global stiffness matrix\n";
    math::Print(m_GlobalStiffnessMatrix * (1.0 / 10e6));

    math::Vector displacements = math::Solve(m_GlobalStiffnessMatrix, forces, m_Progress, m_ProgressMessage);

    std::cout << "Displacements * GSM\n";
    math::Print(displacements * m_GlobalStiffnessMatrix);

    m_ProgressMessage = "Restoring displacements";
    m_Progress = 0.0;
    size_t progressCounter = 0;
    for (auto iter = m_StaticNodes.rbegin(); iter != m_StaticNodes.rend(); ++iter)
    {
        removed--;
        displacements.Join(math::Vector(1.0, 0.0), *iter - removed);
        m_Progress = (double)progressCounter / (double)m_StaticNodes.size();
        progressCounter++;
    }

    std::cout << "Displacements\n";
    math::Print(displacements * 10e6);
    m_ProgressMessage = "Computing stresses";
    m_Progress = 0.0;
    for (size_t i = 0; i < m_Triangles.size(); i++)
    {
        math::Vector displacementVecOfE(6, 0.0);
        displacementVecOfE[0] = displacements[m_Triangles[i]->m_Nodes[0]->m_Index * 2 + 0];
        displacementVecOfE[1] = displacements[m_Triangles[i]->m_Nodes[0]->m_Index * 2 + 1];
        displacementVecOfE[2] = displacements[m_Triangles[i]->m_Nodes[1]->m_Index * 2 + 0];
        displacementVecOfE[3] = displacements[m_Triangles[i]->m_Nodes[1]->m_Index * 2 + 1];
        displacementVecOfE[4] = displacements[m_Triangles[i]->m_Nodes[2]->m_Index * 2 + 0];
        displacementVecOfE[5] = displacements[m_Triangles[i]->m_Nodes[2]->m_Index * 2 + 1];
        m_Triangles[i]->m_XPtr[0] += displacementVecOfE[0];
        m_Triangles[i]->m_XPtr[1] += displacementVecOfE[1];
        m_Triangles[i]->m_XPtr[17 + 0] += displacementVecOfE[2];
        m_Triangles[i]->m_XPtr[17 + 1] += displacementVecOfE[3];
        m_Triangles[i]->m_XPtr[17 * 2 + 0] += displacementVecOfE[4];
        m_Triangles[i]->m_XPtr[17 * 2 + 1] += displacementVecOfE[5];
        m_Triangles[i]->m_Stress += matD * (m_Triangles[i]->m_MatB * displacementVecOfE);
        m_Progress = (double)i / (double)m_Triangles.size();

        //For blending colors among triangles/nodes
        for (size_t j = 0; j < 3; j++)
        {
            for (size_t k = 0; k < 4; k++)
            {
                double stress = abs(m_Triangles[i]->m_Nodes[j]->m_Stress[k]);
                if (stress < abs(m_Triangles[i]->m_Stress[k]))
                    m_Triangles[i]->m_Nodes[j]->m_Stress[k] = m_Triangles[i]->m_Stress[k];
            }
        }
    }

    for (size_t i = 0; i < m_Triangles.size(); i++)
    {
        double Qr = abs(m_Triangles[i]->m_Stress[0]);
        if (Qr > m_MaxStress[0])
            m_MaxStress[0] = Qr;

        double Qz = abs(m_Triangles[i]->m_Stress[1]);
        if (Qz > m_MaxStress[1])
            m_MaxStress[1] = Qz;

        double Trz = abs(m_Triangles[i]->m_Stress[2]);
        if (Trz > m_MaxStress[2])
            m_MaxStress[2] = Trz;

        double Qo = abs(m_Triangles[i]->m_Stress[3]);
        if (Qo > m_MaxStress[3])
            m_MaxStress[3] = Qo;
    }
}