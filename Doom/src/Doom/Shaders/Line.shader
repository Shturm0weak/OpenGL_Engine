#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

uniform mat4 u_Rotate;
uniform mat4 u_ViewProjection;

void main() {
	gl_Position = u_ViewProjection * u_Rotate * positions;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

uniform vec4 U_Color;

void main() {
	gl_FragColor = U_Color;
};