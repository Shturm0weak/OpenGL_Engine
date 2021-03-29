#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 3) in vec3 vertexColor;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 btangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;

void main() {
	vec4 tempFragPos = u_Model * u_View * u_Scale * vec4(positions, 1.0);
	gl_Position = u_ViewProjection * tempFragPos;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

void main()
{
	FragColor = vec4(1.0, 1.0, 0, 1.0);
}