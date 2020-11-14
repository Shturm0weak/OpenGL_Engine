#pragma once

namespace Doom {

	class DOOM_API GridLayOut : public GameObject {

		std::vector<Line*> lines;
		int32_t linesSize = 51;
		float length = 50;
	public:
		GridLayOut() {
			this->name = "Grid";
			this->Enable = false;
			//GetComponentManager()->RemoveComponent<Irenderer>();

			Line::width = 3;
			glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 0.2);

			int32_t half = linesSize * 0.5f;

			for (int32_t i = -half; i < half + 1; i++)
			{
				lines.push_back(new Line(glm::vec3(i, 0, -half), glm::vec3(i, 0, -half + length)));
				lines.back()->color = color;
			}

			for (int32_t i = -half; i < half + 1; i++)
			{
				lines.push_back(new Line(glm::vec3(-half, 0, i), glm::vec3(-half + length, 0, i)));
				lines.back()->color = color;
			}

			lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half + 5, 0.5f, -half)));
			lines.back()->color = COLORS::Red;
			lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f + 5, -half)));
			lines.back()->color = COLORS::Green;
			lines.push_back(new Line(glm::vec3(-half, 0.5f, -half), glm::vec3(-half, 0.5f, -half + 5)));
			lines.back()->color = COLORS::Blue;
			Renderer::PopBack();
			Renderer::obj_id--;
		}

	};
}