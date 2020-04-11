#pragma once

using namespace Doom;

class DOOM_API Line {
public:
	bool UsePosMat = true;
	float width = 1.0f;
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
	Shader* shader = new Shader("src/Shaders/Line.shader");

	glm::vec4 color = COLORS::Red;
	GLuint vao;
	VertexBufferLayout* layout = new VertexBufferLayout;
	VertexBuffer* vb = nullptr;
	VertexArray* va = new VertexArray;
	IndexBuffer* ib = new IndexBuffer(indeces, 2);

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