#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in int m_static;
layout(location = 3) in vec4 m_color;
layout(location = 4) in float isGui;
layout(location = 5) in float isGui;

out vec2 v_textcoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_MVP;

void main() {
	gl_Position = (u_ViewProjection * u_MVP * positions);
	v_textcoords = texcoords;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_textcoords;
uniform vec4 u_Color;
uniform sampler2D u_Texture[32];

void main() {
	vec4 texColor = texture(u_Texture, v_textcoords);
	color = texColor;
};