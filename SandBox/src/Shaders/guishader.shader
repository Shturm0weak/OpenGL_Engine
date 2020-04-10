#shader vertex
#version 330 core


layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in int m_static;
layout(location = 3) in vec4 m_color;

out vec4 out_color;

uniform mat4 u_Projection;

void main() {
	gl_Position = u_Projection * positions;
	out_color = m_color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec4 out_color;
uniform vec4 U_Color;

void main() {
	color = out_color;
};