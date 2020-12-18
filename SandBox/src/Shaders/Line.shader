#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec4 m_color;


uniform mat4 u_ViewProjection;
out vec4 out_color;

void main() {
	gl_Position = u_ViewProjection * positions;
	out_color = m_color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

in vec4 out_color;

void main() {
	FragColor = out_color;
};