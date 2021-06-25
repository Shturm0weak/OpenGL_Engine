#pragma once

#include "Application.h"

namespace Doom
{

	class Tetris : public Application
	{
	private:

		struct Plate
		{
			std::vector<GameObject*> m_Cell;
		};
	public:

		double m_Time = 0.0;
		double m_TimeInDelay = 0.0;
		double m_DelayTime = 0.2;

		size_t m_Score = 0;

		const glm::ivec2 m_Size = { 10, 20 };

		int m_Tetrominos[7][4] =
		{
			1,3,5,7,
			2,4,5,7,
			3,5,4,6,
			3,5,4,7,
			2,3,5,7,
			3,5,7,6,
			2,3,4,5,
		};

		std::vector<GameObject*> cells;

		int m_CurrentPlateIndex;
		bool m_IsRotating = false;
		bool m_Board[10][20];
		bool m_IsGameOver = false;
		std::vector<GameObject*> m_TakenCells[20];
		std::vector<GameObject*> m_GarbageCollector;
		Plate m_CurrentPlate;

		Tetris(std::string name = "Chess", float x = 600, float y = 600, bool Vsync = false) : Application(name, x, y, Vsync) {}

		bool IsCollidedWithBottomBorder();
		bool IsCollidedWithRightBorder();
		bool IsCollidedWithLeftBorder();
		bool IsCellsTaken();
		
		void EraseLine();
		void SpawnTetromino();
		void TakeCells();
		void Rotate();
		void ResetBoard();
		void Restart();

		void OnStart();

		void OnUpdate();

		void OnGuiRender();

		void OnClose();

	};

}