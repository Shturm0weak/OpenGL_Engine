#pragma once

namespace Doom {

	class DOOM_API GridLayOut {
	public:

		static std::vector<Line*> s_GridLines;

		static void DrawGrid(int32_t linesSize, float length)
		{
			
			Line::s_Width = 3;
			glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 0.2);

			int32_t half = linesSize * 0.5f;

			for (int32_t i = -half; i < half + 1; i++)
			{
				s_GridLines.push_back(new Line(glm::vec3(i, 0, -half), glm::vec3(i, 0, -half + length)));
				s_GridLines.back()->m_Color = color;
			}

			for (int32_t i = -half; i < half + 1; i++)
			{
				s_GridLines.push_back(new Line(glm::vec3(-half, 0, i), glm::vec3(-half + length, 0, i)));
				s_GridLines.back()->m_Color = color;
			}

			s_GridLines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half + 5, 0.5f, -half)));
			s_GridLines.back()->m_Color = COLORS::Red;
			s_GridLines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f + 5, -half)));
			s_GridLines.back()->m_Color = COLORS::Green;
			s_GridLines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f, -half + 5)));
			s_GridLines.back()->m_Color = COLORS::Blue;
		}
	};
}