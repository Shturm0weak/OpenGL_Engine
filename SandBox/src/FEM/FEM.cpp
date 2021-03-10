#include "pch.h"
#include "FEM.h"
#include "Text/Gui.h"
#include "Objects/GridLayOut.h"
#include "Integrals.h"

double Doom::FEM::CalculateAreaOfTriangle(const Triangle& triangle)
{
    math::Matrix A(3,3);
    A.AssignCol(math::Vector(3, 1), 0);

    math::Vector col2(3);
    col2[0] = triangle.m_Nodes[0].x;
    col2[1] = triangle.m_Nodes[1].x;
    col2[2] = triangle.m_Nodes[2].x;
    A.AssignCol(col2, 1);

    math::Vector col3(3);
    col3[0] = triangle.m_Nodes[0].y;
    col3[1] = triangle.m_Nodes[1].y;
    col3[2] = triangle.m_Nodes[2].y;
A.AssignCol(col3, 2);

return math::Determinant(A, A.m_Cols) * 0.5;
}

void Doom::FEM::OnStart()
{
    ImGui::SetCurrentContext(Window::GetInstance().s_ImGuiContext);
    Window::GetInstance().GetCamera().SetPosition(glm::vec3(0, 0, 10));
    m_Go = GameObject::Create("Mesh");
    Renderer3D* r3d = m_Go->AddComponent<Renderer3D>();
    r3d->m_IsCullingFace = false;
    GameObject* sun = GameObject::Create("Sun");
    sun->m_Transform.RotateOnce(75, 0, 0);
    sun->AddComponent<DirectionalLight>();
    {
        //weights.push_back({ 0, 128.0 / 225.0 });
        //weights.push_back({ (1.0 / 3.0) * glm::sqrt(5.0 - 2.0 * sqrt(10.0 / 7.0)), (332 + 13 * sqrt(70)) / 900 });
        //weights.push_back({ -(1.0 / 3.0) * glm::sqrt(5.0 - 2.0 * sqrt(10.0 / 7.0)), (332 + 13 * sqrt(70)) / 900 });
        //weights.push_back({ (1.0 / 3.0) * glm::sqrt(5.0 + 2.0 * sqrt(10.0 / 7.0)), (332 - 13 * sqrt(70)) / 900 });
        //weights.push_back({ -(1.0 / 3.0) * glm::sqrt(5.0 + 2.0 * sqrt(10.0 / 7.0)), (332 - 13 * sqrt(70)) / 900 });
        weights.push_back({ 1.0 / 3.0, 1.0 / 3.0, 0.11250 });
        weights.push_back({ 0.0597159, 0.470142,  0.0661971 });
        weights.push_back({ 0.470142,  0.470142,  0.0661971 });
        weights.push_back({ 0.470142,  0.0597159, 0.0661971 });
        weights.push_back({ 0.101287,  0.101287,  0.0629696 });
        weights.push_back({ 0.797427,  0.101287,  0.0629696 });
        weights.push_back({ 0.470142,  0.797427,  0.0629696 });
    }
}

double Doom::FEM::CalculateR(const Triangle& triangle)
{
    return glm::abs((triangle.m_Nodes[0].x + triangle.m_Nodes[1].x + triangle.m_Nodes[2].x) / 3.0);
}

double Doom::FEM::CalculateZ(const Triangle& triangle)
{
    return  glm::abs((triangle.m_Nodes[0].y + triangle.m_Nodes[1].y + triangle.m_Nodes[2].y) / 3.0);
}

void Doom::FEM::OnUpdate()
{
    Window::GetInstance().GetCamera().CameraMovement();
}

glm::dvec3 Doom::FEM::CalculateAlpha(const Triangle& triangle)
{
    glm::dvec3 a;
    a.x = triangle.m_Nodes[1].x * triangle.m_Nodes[2].y - triangle.m_Nodes[2].x * triangle.m_Nodes[1].y;
    a.y = triangle.m_Nodes[2].x * triangle.m_Nodes[0].y - triangle.m_Nodes[0].x * triangle.m_Nodes[2].y;
    a.z = triangle.m_Nodes[0].x * triangle.m_Nodes[1].y - triangle.m_Nodes[1].x * triangle.m_Nodes[0].y;
    return a;
}

glm::dvec3 Doom::FEM::CalculateBeta(const Triangle& triangle)
{
    glm::dvec3 b;
    b.x = triangle.m_Nodes[1].y - triangle.m_Nodes[2].y;
    b.y = triangle.m_Nodes[2].y - triangle.m_Nodes[0].y;
    b.z = triangle.m_Nodes[0].y - triangle.m_Nodes[1].y;
    return b;
}

void Doom::FEM::Clear()
{
    g_Nodes.clear();
    for (size_t i = 0; i < triangles.size(); i++)
    {
        delete triangles[i];
    }
    triangles.clear();
}

glm::dvec3 Doom::FEM::CalculateGamma(const Triangle& triangle)
{
    glm::dvec3 c;
    c.x = triangle.m_Nodes[2].x - triangle.m_Nodes[1].x;
    c.y = triangle.m_Nodes[0].x - triangle.m_Nodes[2].x;
    c.z = triangle.m_Nodes[1].x - triangle.m_Nodes[0].x;
    return c;
}

math::Matrix Doom::FEM::CalculateMatrixJ(Triangle& triangle)
{
    math::Matrix J(2, 2);

    math::Vector col1(2);
    col1[0] = triangle.m_Nodes[0].x - triangle.m_Nodes[2].x;
    col1[1] = triangle.m_Nodes[1].x - triangle.m_Nodes[2].x;
    J.AssignCol(col1, 0);

    math::Vector col2(2);
    col2[0] = triangle.m_Nodes[0].y - triangle.m_Nodes[2].y;
    col2[1] = triangle.m_Nodes[1].y - triangle.m_Nodes[2].y;
    J.AssignCol(col2, 1);

    return J;
}

void Doom::FEM::OnClose()
{
    Clear();
}

void Doom::FEM::OnGuiRender()
{
    Renderer3D* r3d = m_Go->GetComponent<Renderer3D>();
    Mesh* mesh = r3d->m_Mesh;
    Gui& g = Gui::GetInstance();
    g.m_XAlign = g.LEFT;
    g.RelateToPanel();
    g.m_RelatedPanelProperties.m_AutoAllignment = true;
    g.m_RelatedPanelProperties.m_Margin.y = 25;
    g.m_RelatedPanelProperties.m_Margin.x = 25;
    g.m_RelatedPanelProperties.m_Padding.y = 25;
    g.Panel("##1", -900, 200, 350, 600, glm::vec4(0.3, 0.3, 0.3, 0.8));
    if (g.Button("Choose mesh", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
    {
        std::optional<std::string> filePath = FileDialogs::OpenFile("Meshes (*.fbx)\0");
        if (filePath) {
            std::string name = Utils::GetNameFromFilePath(*filePath);
            MeshManager::GetInstance().AsyncLoadMesh(name, *filePath);
            MeshManager::GetInstance().GetMeshWhenLoaded(name, m_Go->GetComponent<Renderer3D>());
        }
    }
    if (mesh == nullptr)
    {
        if (g.Button("Calculate GSM", 0, 0, 30, 300, 75,
            COLORS::Gray * 0.8f * 0.5f, COLORS::Gray * 0.5f * 0.5f, COLORS::White * 0.5f));
    }
    else
    {
        if (g.Button("Calculate GSM", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
        {
            CalculateGlobalStiffnessMatrix(mesh);
        }
    }
    g.CheckBox("Show GSM", &m_ShowStiffnessMatrix, 0, 0, 25);
    g.CheckBox("Is wire mesh", &r3d->m_IsWireMesh, 0, 0, 25);
    if (g.CheckBox("Turn off the grid", &m_TurnOffGrid, 0, 0, 25))
    {
        if (m_TurnOffGrid)
        {
            for (size_t i = 0; i < GridLayOut::s_GridLines.size(); i++)
            {
                GridLayOut::s_GridLines[i]->m_Enable = false;
            }
        }
        else
        {
            for (size_t i = 0; i < GridLayOut::s_GridLines.size(); i++)
            {
                GridLayOut::s_GridLines[i]->m_Enable = true;
            }
        }
       
    }
    if(m_ShowStiffnessMatrix)
    {
        if (ImGui::Begin("Global stiffness Matrix"))
        {
            for (uint32_t i = 0; i < m_GlobalStiffnessMatrix.m_Rows; i++)
            {
                for (uint32_t j = 0; j < m_GlobalStiffnessMatrix.m_Cols; j++)
                {
                    ImGui::Text("%.16lf	", (m_GlobalStiffnessMatrix.m_matrix[i * m_GlobalStiffnessMatrix.m_Cols + j]));
                    ImGui::SameLine();
                }
                ImGui::NewLine();
            }
            ImGui::End();
        }
        if (ImGui::Begin("Global stiffness matrix * Vector"))
        {
            for (uint32_t i = 0; i < m_GSMMulVec.GetSize(); i++)
            {
                ImGui::Text("%.6lf\n", m_GSMMulVec[i]);
            }
            ImGui::End();
        }
    }
    if(g.SliderFloat("Light angle", &m_DirLightAngle, -180.0f, 180.0f, 0, 0, 175, 30))
    {
        World::GetInstance().s_GameObjects[1]->m_Transform.RotateOnce(m_DirLightAngle, 0, 0);
    }
    if (g.Button("Exit", 0, 0, 30, 300, 75, COLORS::Gray * 0.8f))
    {
        Window::GetInstance().Exit();
    }
    g.m_RelatedPanelProperties.m_AutoAllignment = false;
    g.UnRelateToPanel();
    g.Text("FPS %f", true, 800, 450, 40, COLORS::White, 0, Window::GetInstance().GetFPS());
}

void Doom::FEM::AddMatrices(math::Matrix& globalMatK, Triangle* triangle)
{
    std::vector<size_t> localNodes(6);
    localNodes[0] = (triangle->m_NodesIndeces[0] * 2) + 0;
    localNodes[1] = (triangle->m_NodesIndeces[0] * 2) + 1;
    localNodes[2] = (triangle->m_NodesIndeces[1] * 2) + 0;
    localNodes[3] = (triangle->m_NodesIndeces[1] * 2) + 1;
    localNodes[4] = (triangle->m_NodesIndeces[2] * 2) + 0;
    localNodes[5] = (triangle->m_NodesIndeces[2] * 2) + 1;
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
    col[0] = 1 - u;
    col[1] = u;
    col[2] = u;
    col[3] = 0;
    D.AssignCol(col, 0);

    //2
    col[0] = u;
    col[1] = 1 - u;
    col[2] = u;
    col[3] = 0;
    D.AssignCol(col, 1);

    //3
    col[0] = u;
    col[1] = u;
    col[2] = 1 - u;
    col[3] = 0;
    D.AssignCol(col, 2);

    //4
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = (1 - 2 * u) * 0.5;
    D.AssignCol(col, 3);
   
    return D * (E / ((1.0 + u) * (1.0 - 2.0 * u)));
}

math::Matrix Doom::FEM::CalculateMatrixB(double Area, double R, double Z,glm::dvec3 A, glm::dvec3 B, glm::dvec3 Y)
{
    math::Matrix matB(4,6);
    math::Vector col(4);

    //1
    col[0] = B.x;
    col[1] = A.x / R + B.x + (Y.x * Z) / R;
    col[2] = 0;
    col[3] = Y.x;
    matB.AssignCol(col, 0);

    //2
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.x;
    col[3] = B.x;
    matB.AssignCol(col, 1);

    //3
    col[0] = B.y;
    col[1] = A.y / R + B.y + (Y.y * Z) / R;
    col[2] = 0;
    col[3] = Y.y;
    matB.AssignCol(col, 2);

    //4
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.y;
    col[3] = B.y;
    matB.AssignCol(col, 3);

    //5
    col[0] = B.z;
    col[1] = A.z / R + B.z + (Y.z * Z) / R;
    col[2] = 0;
    col[3] = Y.z;
    matB.AssignCol(col, 4);

    //6
    col[0] = 0;
    col[1] = 0;
    col[2] = Y.z;
    col[3] = B.z;
    matB.AssignCol(col, 5);

    return matB * (1 / (2 * Area));
}

bool Doom::FEM::CompareNodes(dvec2 a, dvec2 b)
{
    return (fabs(a.x - b.x) < EPSILON && fabs(a.y - b.y) < EPSILON);
}

void Doom::FEM::IndexNodes(std::vector<Triangle*> triangles)
{
    for (size_t i = 0; i < triangles.size(); i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            auto iter = std::find_if(g_Nodes.begin(), g_Nodes.end(), [=](dvec2& a) { return CompareNodes(a, triangles[i]->m_Nodes[j]); });
            if (iter == g_Nodes.end()) {
                g_Nodes.push_back(triangles[i]->m_Nodes[j]);
            }
        }
    }
    maxDimension = g_Nodes.size() * 2;
    for (size_t i = 0; i < triangles.size(); i++) 
    {
        for (size_t j = 0; j < g_Nodes.size(); j++)
        {
            for (size_t k = 0; k < 3; k++)
            {
                if (CompareNodes(triangles[i]->m_Nodes[k], g_Nodes[j]))
                {
                    triangles[i]->m_NodesIndeces.push_back(j);
                    break;
                }
            }
        }
    }
}

void Doom::FEM::CalculateGlobalStiffnessMatrix(Mesh* mesh)
{
    Clear();
    mesh->m_VertAttrib;
    for (uint32_t i = 0; i < mesh->m_VertAttribSize; i += (14 * 3))
    {
        glm::dvec2 a = glm::dvec2(mesh->m_VertAttrib[i + 0], mesh->m_VertAttrib[i + 1]);
        glm::dvec2 b = glm::dvec2(mesh->m_VertAttrib[i + 14 + 0], mesh->m_VertAttrib[i + 14 + 1]);
        glm::dvec2 c = glm::dvec2(mesh->m_VertAttrib[i + 28 + 0], mesh->m_VertAttrib[i + 28 + 1]);
        triangles.push_back(new Triangle(a, b, c));
    }
    IndexNodes(triangles);
    m_GlobalStiffnessMatrix = math::Matrix(maxDimension, maxDimension);
    math::Matrix matD = CalculateMatrixD(m_E, m_U);
    for (size_t i = 0; i < triangles.size(); i++)
    {
        Triangle* triangle = triangles[i];
        double area = CalculateAreaOfTriangle(*triangle);
        double R = CalculateR(*triangle);
        double Z = CalculateZ(*triangle);
        glm::dvec3 A = CalculateAlpha(*triangle);
        glm::dvec3 B = CalculateBeta(*triangle);
        glm::dvec3 Y = CalculateGamma(*triangle);
        //math::Matrix matJ = CalculateMatrixJ(*triangle);
        math::Matrix matB = CalculateMatrixB(area, R, Z, A, B, Y);
        math::Matrix matK = (math::Transpose(matB) * (matD * matB));
        triangle->m_MatK = matK * 2 * glm::pi<double>() * R * area;
        std::cout << "Approx" << std::endl;
        math::Print(triangle->m_MatK);
        //double detJ = math::Determinant(matJ, matJ.m_Rows);
        triangle->m_MatK = math::Matrix(matK.m_Rows, matK.m_Cols);
        for (size_t u = 0; u < triangle->m_MatK.m_Rows; u++)
        {
            for (size_t j = 0; j < triangle->m_MatK.m_Cols; j++)
            {
                double s = 0;
                for (size_t k = 0; k < 7; k++)
                {
                    math::Matrix matBTest = CalculateMatrixB(area, weights[k].r, weights[k].z, A, B, Y);
                    math::Matrix matKTest = (math::Transpose(matBTest) * (matD * matBTest));
                    s += (matKTest.operator()(u, j) * weights[k].r * weights[k].w * 2 * area);
                   
                }
                triangle->m_MatK.operator()(u, j) += s * glm::pi<double>() * 2.0;
            }
        }
        std::cout << "Exact" << std::endl;
        math::Print(triangle->m_MatK);
        AddMatrices(m_GlobalStiffnessMatrix, triangle);
    }

    math::Vector vec(m_GlobalStiffnessMatrix.m_Rows);
    for (size_t i = 0; i < m_GlobalStiffnessMatrix.m_Rows; i+=2)
    {
        vec[i] = 0;
        vec[i + 1] = 1;
    }
    m_GSMMulVec = m_GlobalStiffnessMatrix * vec;
}
