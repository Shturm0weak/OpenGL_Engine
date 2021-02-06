#pragma once

namespace Doom {

	class DOOM_API GridLayOut : public GameObject {
	private:

		std::vector<Line*> m_Lines;
		int32_t m_LinesSize = 51;
		float m_Length = 50;
	public:

		GridLayOut() 
		{
			this->m_Name = "Grid";
			this->m_Enable = false;
			Line::s_Width = 3;
			glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 0.2);

			int32_t half = m_LinesSize * 0.5f;

			for (int32_t i = -half; i < half + 1; i++)
			{
				m_Lines.push_back(new Line(glm::vec3(i, 0, -half), glm::vec3(i, 0, -half + m_Length)));
				m_Lines.back()->m_Color = color;
			}

			for (int32_t i = -half; i < half + 1; i++)
			{
				m_Lines.push_back(new Line(glm::vec3(-half, 0, i), glm::vec3(-half + m_Length, 0, i)));
				m_Lines.back()->m_Color = color;
			}

			m_Lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half + 5, 0.5f, -half)));
			m_Lines.back()->m_Color = COLORS::Red;
			m_Lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f + 5, -half)));
			m_Lines.back()->m_Color = COLORS::Green;
			m_Lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f, -half + 5)));
			m_Lines.back()->m_Color = COLORS::Blue;
			World::GetInstance().PopBack();
			World::GetInstance().s_ObjId--;
		}
	};
}