#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec4 color;

out vec4 out_color;
uniform mat4 u_MVP;
uniform mat4 u_ViewProjection;
//uniform vec4 m_color;

void main() {
	gl_Position = u_ViewProjection * u_MVP * vec4(positions,1);
	out_color = color;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
in vec4 out_color;

void main() {
	gl_FragColor = out_color;
};