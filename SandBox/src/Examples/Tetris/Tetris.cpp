#include "pch.h"
#include "Tetris.h"
#include "Text/Gui.h"

void Doom::Tetris::OnStart()
{
    Camera& camera = Window::GetInstance().GetCamera();
    camera.SetOrthographic(16.0f / 9.0f);
    camera.Zoom(12.5f);
    camera.SetPosition(glm::vec3(5.0f, 10.0f, 0.0f));
    for (size_t i = 0; i < m_Size.x; i++)
    {
        for (size_t j = 0; j < m_Size.y; j++)
        {
            m_Board[i][j] = false;
            GameObject* cell = GameObject::Create("cell", i, j);
            cell->AddComponent<SpriteRenderer>()->m_Color = m_Board[i][j] ? COLORS::Red : COLORS::Blue;
            cell->m_Transform.Scale(glm::vec3(0.2f));
            cells.push_back(cell);
        }
    }
    SpawnTetromino();
}

void Doom::Tetris::OnUpdate()
{
    m_Time += DeltaTime::s_Deltatime;
    if (m_IsGameOver)
    {
        if (Input::IsKeyPressed(Keycode::KEY_ENTER))
        {
            Restart();
        }
        else return;
    }
    m_TimeInDelay += DeltaTime::s_Deltatime;
    if (Input::IsKeyPressed(Keycode::KEY_A))
    {
        for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
        {
            m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() - glm::vec3(1.0f, 0.0f, 0.0f));  
        }
        if (IsCollidedWithLeftBorder() || IsCellsTaken())
        {
            for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
            {
                m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
    }
    else if (Input::IsKeyPressed(Keycode::KEY_D))
    {
        for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
        {
            m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (IsCollidedWithRightBorder() || IsCellsTaken())
        {
            for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
            {
                m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() - glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
    }
    if (Input::IsKeyPressed(Keycode::KEY_R))
    {
        m_IsRotating = true;
    }
    if (m_TimeInDelay > m_DelayTime)
    {
        m_TimeInDelay = 0.0;
        if (m_IsRotating)
        {
            Rotate();
        }
        for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
        {
            m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() - glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (IsCellsTaken() || IsCollidedWithBottomBorder())
        {
            for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
            {
                m_CurrentPlate.m_Cell[i]->m_Transform.Translate(m_CurrentPlate.m_Cell[i]->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f));
            }
            TakeCells();
            EraseLine();
            m_CurrentPlate.m_Cell.clear();
            if (m_IsGameOver) return;
            SpawnTetromino();
        }
    }
}

void Doom::Tetris::OnGuiRender()
{
    Gui& g = Gui::GetInstance();
    glm::vec2 size = ViewPort::GetInstance().GetSize();
    float ratio = ViewPort::GetInstance().GetAspectRatio();
    g.m_XAlign = g.XCENTER;
    g.Text(L"Score: %d", true, 0.0f, (size[1] * 1.5) / ratio, 50.0f, COLORS::White, 0, m_Score);
}

void Doom::Tetris::OnClose()
{

}

bool Doom::Tetris::IsCollidedWithBottomBorder()
{
    for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
    {
        if (glm::epsilonEqual(m_CurrentPlate.m_Cell[i]->GetPosition().y, -1.0f, 0.001f))
        {
            return true;
        }
    }
    return false;
}

bool Doom::Tetris::IsCollidedWithRightBorder()
{
    for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
    {
        if (glm::epsilonEqual(m_CurrentPlate.m_Cell[i]->GetPosition().x, (float)m_Size.x, 0.001f))
        {
            return true;
        }
    }
    return false;
}

bool Doom::Tetris::IsCollidedWithLeftBorder()
{
    for (size_t i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
    {
        if (glm::epsilonEqual(m_CurrentPlate.m_Cell[i]->GetPosition().x, -1.0f, 0.001f))
        {
            return true;
        }
    }
    return false;
}

bool Doom::Tetris::IsCellsTaken()
{
    for (size_t k = 0; k < m_CurrentPlate.m_Cell.size(); k++)
    {
        glm::ivec3 pos = m_CurrentPlate.m_Cell[k]->GetPosition();
        if (m_Board[(int)pos.x][(int)pos.y]) return true;
    }
    return false;
}

void Doom::Tetris::EraseLine()
{
    for (int i = 0; i < m_Size.y; i++)
    {
        bool isAllTaken = true;
        for (size_t j = 0; j < m_Size.x; j++)
        {
            if (m_Board[j][i] == false)
            {
                isAllTaken = false;
            }
        }
        if (isAllTaken)
        {
            for (size_t j = 0; j < m_TakenCells[i].size(); j++)
            {
                m_TakenCells[i][j]->m_ComponentManager.Clear();
            }
            m_TakenCells[i].clear();
            for (size_t j = i; j < m_Size.y - 1; j++)
            {
                m_TakenCells[j].clear();
                m_TakenCells[j] = m_TakenCells[j + 1];
            }
            for (size_t j = i; j < m_Size.y; j++)
            {
                for (size_t k = 0; k < m_TakenCells[j].size(); k++)
                {
                    m_TakenCells[j][k]->m_Transform.Translate(m_TakenCells[j][k]->GetPosition() - glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            ResetBoard();
            for (size_t j = 0; j < m_Size.y; j++)
            {
                for (size_t k = 0; k < m_TakenCells[j].size(); k++)
                {
                    glm::vec3 pos = m_TakenCells[j][k]->GetPosition();
                    m_Board[(int)pos.x][(int)pos.y] = true;
                }
            }
            i--;
            m_Score += 100;
        }
    }
}

void Doom::Tetris::SpawnTetromino()
{
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_int_distribution<int> position(2, m_Size.x - 2);
    int x = position(e2);
    std::uniform_int_distribution<int> index(0, 6);
    int n = index(e2);

    glm::vec4 color(1.0f);
    std::uniform_real_distribution<float> r(0.3f, 1.0f);
    color.x = r(e2);
    std::uniform_real_distribution<float> g(0.3f, 1.0f);
    color.y = g(e2);
    std::uniform_real_distribution<float> b(0.3f, 1.0f);
    color.z = b(e2);

    m_CurrentPlateIndex = n;
    for (size_t i = 0; i < 4; i++)
    {
        GameObject* go = GameObject::Create(std::to_string(i), (m_Tetrominos[n][i] % 2), (m_Tetrominos[n][i] / 2));
        go->AddComponent<SpriteRenderer>()->m_Color = color;
        go->m_Transform.Translate(go->GetPosition() + glm::vec3(x, m_Size.y - 4, 0.0f));
        go->m_Transform.Scale(glm::vec3(0.95f));
        m_CurrentPlate.m_Cell.push_back(go);
        m_GarbageCollector.push_back(go);
    }
}

void Doom::Tetris::TakeCells()
{
    m_Score += 20;
    for (size_t k = 0; k < m_CurrentPlate.m_Cell.size(); k++)
    {
        glm::ivec3 pos = m_CurrentPlate.m_Cell[k]->GetPosition();
        if (pos.y == m_Size.y - 1)
        {
            m_IsGameOver = true;
            return;
        }
        m_Board[(int)pos.x][(int)pos.y] = true;
        m_TakenCells[(int)pos.y].push_back(m_CurrentPlate.m_Cell[k]);
    }
    for (size_t i = 0; i < m_Size.x; i++)
    {
        for (size_t j = 0; j < m_Size.y; j++)
        {
            cells[i * m_Size.y + j]->GetComponent<SpriteRenderer>()->m_Color = m_Board[i][j] ? COLORS::Red : COLORS::Blue;
        }
    }
}

void Doom::Tetris::Rotate()
{
    glm::ivec2 p = m_CurrentPlate.m_Cell[1]->GetPosition();
    glm::ivec2 previousPosition[4];
    for (int i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
    {
        glm::ivec2 a = m_CurrentPlate.m_Cell[i]->GetPosition();
        previousPosition[i] = a;
        glm::ivec2 pos = { p.x - (a.y - p.y), p.y + (a.x - p.x) };
        m_CurrentPlate.m_Cell[i]->m_Transform.Translate(glm::vec3(pos, 0.0f));
    }
    if (IsCollidedWithLeftBorder() || IsCollidedWithRightBorder() || IsCellsTaken())
    {
        for (int i = 0; i < m_CurrentPlate.m_Cell.size(); i++)
        {
            m_CurrentPlate.m_Cell[i]->m_Transform.Translate(glm::vec3(previousPosition[i], 0.0f));
        }
    }
    m_IsRotating = false;
}

void Doom::Tetris::ResetBoard()
{
    for (size_t i = 0; i < m_Size.x; i++)
    {
        for (size_t j = 0; j < m_Size.y; j++)
        {
            m_Board[i][j] = false;
            cells[i * m_Size.y + j]->GetComponent<SpriteRenderer>()->m_Color = m_Board[i][j] ? COLORS::Red : COLORS::Blue;
        }
    }
}

void Doom::Tetris::Restart()
{
    for (size_t i = 0; i < m_Size.y; i++)
    {
        for (size_t j = 0; j < m_TakenCells[i].size(); j++)
        {
            m_TakenCells[i][j]->m_ComponentManager.Clear();
        }
        m_TakenCells[i].clear();
    }
    for (size_t i = 0; i < m_GarbageCollector.size(); i++)
    {
        World::GetInstance().DeleteObject(m_GarbageCollector[i]->m_Id);
    }
    m_GarbageCollector.clear();
    m_Score = 0;
    m_IsGameOver = false;
    ResetBoard();
    SpawnTetromino();
}
