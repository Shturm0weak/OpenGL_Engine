#shader vertex
#version 330 core


layout(location = 0) in vec4 positions;
layout(location = 5) in vec4 rotationMat0;
layout(location = 6) in vec4 rotationMat1;
layout(location = 7) in vec4 rotationMat2;
layout(location = 8) in vec4 rotationMat3;

uniform mat4 u_ViewProjection;
uniform mat4 u_MVP;
mat4 rot = mat4(
	rotationMat0,
	rotationMat1,
	rotationMat2,
	rotationMat3);

void main() 
{
	gl_Position = u_ViewProjection * rot * positions;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 U_Color;

void main() 
{
	color = U_Color;
}