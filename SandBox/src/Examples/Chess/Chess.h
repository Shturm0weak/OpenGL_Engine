#pragma once

#include "Application.h"

namespace Doom
{

	class Chess : public Application 
	{
	private:

		enum class Type
		{
			PAWN,
			BISHOP,
			KNIGHT,
			ROOK,
			QUEEN,
			KING
		};

		enum class Team
		{
			WHITETEAM,
			BLACKTEAM
		};

		enum class State
		{
			TURN,
			SELECT
		};

		class Cell;
		class Figure;

		class Figure : public Component
		{
		public:
			static std::vector<Figure*> m_Figures;
			Cell* m_Parent = nullptr;
			Type m_Name;
			Team m_Team;

			Figure() { m_Figures.push_back(this); }
			static Component* Create();
			virtual void Delete() override {};
			virtual void CheckValidMoves(Chess& chess) {};
			virtual void Move(Chess& chess) {};
			friend class Chess;
		};

		class Pawn : public Figure
		{
		public:
			static Component* Create() { return new Pawn(); }
			virtual void CheckValidMoves(Chess& chess) override;
			virtual void Move(Chess& chess) override;
		};

		class Cell : public Component
		{
		private:

			static size_t s_CellsCounter;
			static Cell s_Cells[64];
		public:

			glm::vec4 m_DefaultColor;
			glm::ivec2 m_PositionOnGrid;
			GameObject* m_Figure = nullptr;

			static Component* Create();
			virtual void Delete() override {};

			friend class Chess;
		};

	public:

		Window& m_Window = Window::GetInstance();
		Camera& m_Camera = Window::GetInstance().GetCamera();
		std::vector<GameObject*> m_ChessBoard;
		TextureAtlas* m_FiguresAtlas = nullptr;
		std::vector<std::string> m_Mask;
		std::vector<GameObject*> m_HighLightedCells;
		GameObject* m_SelectedFigure = nullptr;
		State m_GameState = State::SELECT;
		bool m_IsFirstTurn = true;
		bool m_IsWhiteTurn = true;

		Chess(std::string name = "Chess", float x = 800, float y = 600, bool Vsync = false) : Application(name, x, y, Vsync) {}

		void GenerateChessBoard();
		void CalculatePawnTurns(Figure* figure);
		void MovePawn();
		void ClearCellsColor();
		
		void OnStart();

		void OnUpdate();

		void OnGuiRender();

		void OnClose();

	};

}