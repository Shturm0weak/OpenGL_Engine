#include "pch.h"
#include "Chess.h"
#include "Text/Gui.h"

using namespace Doom;

size_t Chess::Cell::s_CellsCounter = 0;
Chess::Cell Chess::Cell::s_Cells[64];

std::vector<Chess::Figure*> Chess::Figure::m_Figures;

void Chess::GenerateChessBoard()
{
	for (size_t i = 0; i < 8; i++)
	{
		for (size_t j = 0; j < 8; j++)
		{
			GameObject* cell = GameObject::Create("Cell:" + std::to_string(i) + ":" + std::to_string(j), i, j);
			cell->m_Tag = "Cell";
			cell->m_Transform.Scale(glm::vec3(0.9f, 0.9f, 0.0f));
			Cell* cellCom = cell->AddComponent<Cell>();
			cellCom->m_PositionOnGrid = glm::ivec2(i, j);
			SpriteRenderer* sr = cell->AddComponent<SpriteRenderer>();
			glm::vec4 color1 = glm::vec4(220.0f / 255.0f, 145.0f / 255.0f, 60.0f / 255.0f, 1.0f);
			glm::vec4 color2 = glm::vec4(1.0f, 200.0f / 255.0f, 137.0f / 255.0f, 1.0f);
			cellCom->m_DefaultColor = (i + j) % 2 == 0 ? color1 : color2;
			sr->m_Color = cellCom->m_DefaultColor;
			
			GameObject* figure = nullptr;
			if (j < 2 || j > 5)
			{
				figure = GameObject::Create("Figure:" + std::to_string(i) + ":" + std::to_string(j), i, j);
				sr = figure->AddComponent<SpriteRenderer>();
				sr->m_Texture = m_FiguresAtlas->GetTexture();
				if (j == 1)
				{
					sr->SetUVs(m_FiguresAtlas->GetSpriteUVs(5, 0));
					Pawn* figureCom = figure->AddComponent<Pawn>();
					figureCom->m_Parent = cell->GetComponent<Cell>();
					figureCom->m_Name = Type::PAWN;
					figureCom->m_Team = Team::BLACKTEAM;
				}
				else if (j == 6)
				{
					sr->SetUVs(m_FiguresAtlas->GetSpriteUVs(5, 1));
					Pawn* figureCom = figure->AddComponent<Pawn>();
					figureCom->m_Parent = cell->GetComponent<Cell>();
					figureCom->m_Name = Type::PAWN;
					figureCom->m_Team = Team::WHITETEAM;
				}
				figure->m_Transform.Move(glm::vec3(0.0f, 0.0, 1000.0f));
				cell->GetComponent<Cell>()->m_Figure = figure;
			}
			m_ChessBoard.push_back(cell);
		}
	}

	auto AddFigureToCell = [](std::vector<GameObject*>& chessBoard, int cellId, TextureAtlas* textureAtlas, glm::vec2 spriteId, Type type)
	{
		Cell* cell = chessBoard[cellId]->GetComponent<Cell>();
		Figure* figure = cell->m_Figure->AddComponent<Figure>();
		figure->m_Name = type;
		figure->m_Parent = cell;
		cell->m_Figure = figure->m_OwnerOfCom;
		cell->m_Figure->GetComponent<SpriteRenderer>()->SetUVs(textureAtlas->GetSpriteUVs(spriteId.x, spriteId.y));
	};

	//black
	
	AddFigureToCell(m_ChessBoard, 0 * 8 + 0, m_FiguresAtlas, glm::vec2(4, 0), Type::ROOK);
	AddFigureToCell(m_ChessBoard, 1 * 8 + 0, m_FiguresAtlas, glm::vec2(3, 0), Type::KNIGHT);
	AddFigureToCell(m_ChessBoard, 2 * 8 + 0, m_FiguresAtlas, glm::vec2(2, 0), Type::BISHOP);
	AddFigureToCell(m_ChessBoard, 3 * 8 + 0, m_FiguresAtlas, glm::vec2(0, 0), Type::KING);
	AddFigureToCell(m_ChessBoard, 4 * 8 + 0, m_FiguresAtlas, glm::vec2(1, 0), Type::QUEEN);
	AddFigureToCell(m_ChessBoard, 5 * 8 + 0, m_FiguresAtlas, glm::vec2(2, 0), Type::BISHOP);
	AddFigureToCell(m_ChessBoard, 6 * 8 + 0, m_FiguresAtlas, glm::vec2(3, 0), Type::KNIGHT);
	AddFigureToCell(m_ChessBoard, 7 * 8 + 0, m_FiguresAtlas, glm::vec2(4, 0), Type::ROOK);

	//white

	AddFigureToCell(m_ChessBoard, 0 * 8 + 7, m_FiguresAtlas, glm::vec2(4, 1), Type::ROOK);
	AddFigureToCell(m_ChessBoard, 1 * 8 + 7, m_FiguresAtlas, glm::vec2(3, 1), Type::KNIGHT);
	AddFigureToCell(m_ChessBoard, 2 * 8 + 7, m_FiguresAtlas, glm::vec2(2, 1), Type::BISHOP);
	AddFigureToCell(m_ChessBoard, 3 * 8 + 7, m_FiguresAtlas, glm::vec2(0, 1), Type::KING);
	AddFigureToCell(m_ChessBoard, 4 * 8 + 7, m_FiguresAtlas, glm::vec2(1, 1), Type::QUEEN);
	AddFigureToCell(m_ChessBoard, 5 * 8 + 7, m_FiguresAtlas, glm::vec2(2, 1), Type::BISHOP);
	AddFigureToCell(m_ChessBoard, 6 * 8 + 7, m_FiguresAtlas, glm::vec2(3, 1), Type::KNIGHT);
	AddFigureToCell(m_ChessBoard, 7 * 8 + 7, m_FiguresAtlas, glm::vec2(4, 1), Type::ROOK);

	for (size_t i = 0; i < Figure::m_Figures.size(); i++)
	{
		Figure::m_Figures[i]->m_OwnerOfCom->GetComponent<SpriteRenderer>()->SetFrontLayer();
		Figure::m_Figures[i]->m_Type = Utils::GetComponentTypeName<Figure>();
		if (Figure::m_Figures[i]->m_Team == Team::WHITETEAM)
		{
			Figure::m_Figures[i]->m_OwnerOfCom->m_Tag = "WhiteTeam";
		}
		else if (Figure::m_Figures[i]->m_Team == Team::BLACKTEAM)
		{
			Figure::m_Figures[i]->m_OwnerOfCom->m_Tag = "BlackTeam";
		}
	}
}

void Chess::OnStart()
{
	int* size = m_Window.GetSize();
	m_Camera.SetOrthographic((float)size[0] / (float)size[1]);
	m_Camera.SetPosition(glm::vec3(4.0f, 4.0f, 0.0f));
	m_Camera.Zoom(5.0f);
	m_FiguresAtlas = TextureAtlas::CreateTextureAtlas("ChessFigures", 425, 425, Texture::Create("src/Examples/Chess/Images/Figures.png"));
	GenerateChessBoard();
}

void Chess::OnUpdate()
{
	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (m_GameState == State::SELECT)
		{
			m_Mask.clear();
			std::string turnTag = m_IsWhiteTurn ? "WhiteTeam" : "BlackTeam";
			m_Mask.push_back(turnTag);
			GameObject* previousSelectedFigure = m_SelectedFigure;
			m_SelectedFigure = World::GetInstance().SelectObject(m_Mask);
			if (m_SelectedFigure != nullptr)
			{
				ClearCellsColor();
				if (previousSelectedFigure != nullptr)
				{
					GameObject* parentCell = previousSelectedFigure->GetComponent<Figure>()->m_Parent->m_OwnerOfCom;
					parentCell->GetComponent<SpriteRenderer>()->m_Color = parentCell->GetComponent<Cell>()->m_DefaultColor;
				}

				SpriteRenderer* sr = m_SelectedFigure->GetComponent<Figure>()->m_Parent->GetComponent<SpriteRenderer>();
				sr->m_Color = COLORS::Blue;
				m_SelectedFigure->GetComponent<Figure>()->CheckValidMoves(*this);
				m_GameState = State::TURN;
			}
			else m_SelectedFigure = previousSelectedFigure;
		}
		else if (m_GameState == State::TURN)
		{
			m_Mask.clear();
			m_Mask.push_back("Selected");
			std::string previousTag = m_SelectedFigure->m_Tag;
			m_SelectedFigure->m_Tag = "Selected";
			GameObject* previousSelectedFigure = m_SelectedFigure;
			m_SelectedFigure = World::GetInstance().SelectObject(m_Mask);
			if (m_SelectedFigure == previousSelectedFigure)
			{
				ClearCellsColor();
				m_GameState = State::SELECT;
				GameObject* parentCell = m_SelectedFigure->GetComponent<Figure>()->m_Parent->m_OwnerOfCom;
				parentCell->GetComponent<SpriteRenderer>()->m_Color = parentCell->GetComponent<Cell>()->m_DefaultColor;
			}
			else
			{
				m_SelectedFigure = previousSelectedFigure;
				m_SelectedFigure->GetComponent<Figure>()->Move(*this);
			}
			m_SelectedFigure->m_Tag = previousTag;
		}
	}
}

void Chess::OnGuiRender()
{

}

void Chess::OnClose()
{
}

void Chess::ClearCellsColor()
{
	for (size_t i = 0; i < m_HighLightedCells.size(); i++)
	{
		m_HighLightedCells[i]->GetComponent<SpriteRenderer>()->m_Color = m_HighLightedCells[i]->GetComponent<Cell>()->m_DefaultColor;
	}
	m_HighLightedCells.clear();
}

Component* Chess::Cell::Create()
{
	return &s_Cells[s_CellsCounter++];
}

Component* Chess::Figure::Create()
{
	return new Figure();
}

void Doom::Chess::Pawn::CheckValidMoves(Chess& chess)
{
	int direction = m_Team == Team::WHITETEAM ? -1 : 1;

	int MinMaxCol = m_Parent->m_PositionOnGrid.y + direction;

	if (MinMaxCol > -1 && MinMaxCol < 8)
	{
		GameObject* cellObject = chess.m_ChessBoard[m_Parent->m_PositionOnGrid.x * 8 + (m_Parent->m_PositionOnGrid.y + direction)];
		if (cellObject->GetComponent<Cell>()->m_Figure == nullptr)
		{
			cellObject->GetComponent<SpriteRenderer>()->m_Color = COLORS::Red;
			chess.m_HighLightedCells.push_back(cellObject);
			if (chess.m_IsFirstTurn)
			{
				cellObject = chess.m_ChessBoard[m_Parent->m_PositionOnGrid.x * 8 + (m_Parent->m_PositionOnGrid.y + direction * 2)];
				if (cellObject->GetComponent<Cell>()->m_Figure == nullptr)
				{
					cellObject->GetComponent<SpriteRenderer>()->m_Color = COLORS::Red;
					chess.m_HighLightedCells.push_back(cellObject);
				}
			}
		}
	}
}

void Doom::Chess::Pawn::Move(Chess& chess)
{
	chess.m_Mask.clear();
	chess.m_Mask.push_back("Cell");
	GameObject* m_SelectedCell = World::GetInstance().SelectObject(chess.m_Mask);
	for (size_t i = 0; i < chess.m_HighLightedCells.size(); i++)
	{
		if (m_SelectedCell == chess.m_HighLightedCells[i])
		{
			if (m_SelectedCell->GetComponent<Cell>()->m_Figure != nullptr)
			{
				Figure* figureDestroyedCom = m_SelectedCell->GetComponent<Cell>()->m_Figure->GetComponent<Figure>();
				figureDestroyedCom->m_Parent->GetComponent<Cell>()->m_Figure = nullptr;
				figureDestroyedCom->m_Parent = nullptr;
				figureDestroyedCom->m_OwnerOfCom->m_Enable = false;
			}
			glm::vec3 cellPos = m_SelectedCell->GetPosition();
			glm::vec3 newPos = glm::vec3(cellPos.x, cellPos.y, chess.m_SelectedFigure->GetPosition().z);
			chess.m_IsWhiteTurn = !chess.m_IsWhiteTurn;
			chess.m_IsFirstTurn = false;
			chess.m_GameState = State::SELECT;
			chess.m_SelectedFigure->m_Transform.Translate(newPos);
			Figure* figureCom = chess.m_SelectedFigure->GetComponent<Figure>();
			figureCom->m_Parent->GetComponent<SpriteRenderer>()->m_Color = figureCom->m_Parent->GetComponent<Cell>()->m_DefaultColor;
			//figureCom->m_Parent->RemoveChild(m_SelectedFigure);
			figureCom->m_Parent->GetComponent<Cell>()->m_Figure = nullptr;
			figureCom->m_Parent = m_SelectedCell->GetComponent<Cell>();
			m_SelectedCell->GetComponent<Cell>()->m_Figure = chess.m_SelectedFigure;
			//m_SelectedCell->AddChild(m_SelectedFigure);
			chess.m_GameState = State::SELECT;
			chess.ClearCellsColor();
		}
	}
}
