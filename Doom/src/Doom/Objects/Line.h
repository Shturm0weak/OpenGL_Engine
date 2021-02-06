#pragma once

using namespace Doom;

class DOOM_API Line {
public:

	glm::mat4 m_PosMat4 = glm::mat4(1.f);
	glm::mat4 m_ViewMat4 = glm::mat4(1.f);
	float m_Verteces[6] = {
		0,0,0,
		0,0,0
	};
	glm::vec4 m_Color = COLORS::Red;
	glm::vec3 m_Start;
	glm::vec3 m_End;
	glm::vec3 m_Dir;
	const unsigned int m_Indices[2] = { 0,1 };
	Shader* m_Shader = nullptr;
	float m_Angle;
	float m_MaxLength = 100;
	bool m_IsUsingPosMat4 = true;
	bool m_Static = false;
	bool m_Enable = true;

	static float s_Width;
	static std::vector<Line*> s_Lines;

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