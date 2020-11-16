#pragma once

using namespace Doom;

class DOOM_API Line {
public:
	bool UsePosMat = true;
	bool Static = false;
	bool Enable = true;
	float angle;
	float maxLength = 100;
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 direction;
	const unsigned int indeces[2] = { 0,1 };
	float mesh2D[6] = {
		0,0,0,
		0,0,0
	};

	static float width;
	static std::vector<Line*> lines;

	Shader* shader = nullptr;

	glm::vec4 color = COLORS::Red;
	glm::mat4 pos = glm::mat4(1.f);
	glm::mat4 MVP = glm::mat4(1.f);
	glm::mat4 view = glm::mat4(1.f);

	Line(glm::vec3 start,glm::vec3 end);
	Line(glm::vec3 start,glm::vec3 direction,float maxLength);
	Line(glm::vec3 start, float angle, float maxLength);

	void SetEndPoint(glm::vec3 pos);
	void SetEndPoint(float* pos);
	void SetEndPoint(float x, float y,float z = 0);
	void SetStartPoint(float* pos);
	void SetStartPoint(glm::vec3 pos);
	void SetStartPoint(float x, float y,float z = 0);
	void Rotate(float angle);
};