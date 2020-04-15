#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

uniform mat4 u_Rot;
uniform mat4 u_ViewProjection;
uniform mat4 u_MVP;

void main() {
	gl_Position = u_ViewProjection * u_MVP * u_Rot * positions;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

uniform vec4 U_Color;

void main() {
	gl_FragColor = U_Color;
};