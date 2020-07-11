#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;

uniform mat4 u_Model;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;

void main() {
	gl_Position = u_ViewProjection * u_Model * u_Scale * vec4(positions, 1.0);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

uniform vec4 u_Color;

void main() {
	gl_FragColor = u_Color;
}