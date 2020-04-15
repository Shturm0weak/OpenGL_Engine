#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 6) in vec4 MVP0;
layout(location = 7) in vec4 MVP1;
layout(location = 8) in vec4 MVP2;
layout(location = 9) in vec4 MVP3;
layout(location = 2) in vec4 ROT0;
layout(location = 3) in vec4 ROT1;
layout(location = 4) in vec4 ROT2;
layout(location = 5) in vec4 ROT3;
layout(location = 10) in vec4 m_color;

//uniform mat4 u_Rot;
uniform mat4 u_ViewProjection;
//uniform mat4 u_MVP;

mat4 rot = mat4(
	ROT0,
	ROT1,
	ROT2,
	ROT3);

mat4 mvp = mat4(
	MVP0,
	MVP1,
	MVP2,
	MVP3);

out vec4 out_color;

void main() {
	gl_Position = u_ViewProjection * positions;
	out_color = m_color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

in vec4 out_color;

void main() {
	gl_FragColor = out_color;
};