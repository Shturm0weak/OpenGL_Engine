#pragma once

using namespace Doom;

class DOOM_API Line {
public:
	bool UsePosMat = true;
	static float width;
	bool Static = false;
	bool Enable = true;
	float angle;
	glm::vec2 start;
	glm::vec2 end;
	glm::vec2 direction;
	float maxLength = 100;
	const unsigned int indeces[2] = { 0,1 };
	float mesh2D[4] = {

		0,0,
		0,0

	};

	static std::vector<Line*> lines;
	Shader* shader = nullptr;

	glm::vec4 color = COLORS::Red;

	glm::mat4 pos = glm::mat4(1.f);
	glm::mat4 MVP = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);

	Line(glm::vec2 start,glm::vec2 end);
	Line(glm::vec2 start,glm::vec2 direction,float maxLength);
	Line(glm::vec2 start, float angle, float maxLength);

	void OnRunning();

	void SetEndPoint(float* pos);
	void SetEndPoint(float x, float y);
	void SetStartPoint(float* pos);
	void SetStartPoint(float x, float y);
	void Rotate(float angle);
};