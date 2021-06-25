#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;

void main() 
{
	gl_Position = u_ViewProjection * u_Model * u_View * u_Scale * vec4(positions, 1);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
float zNear = 0.1;
float zFar = 100.0;
in highp vec4 gl_FragCoord;
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
	gl_FragColor = vec4(vec3(1 - depth,depth,depth),1.0);
}