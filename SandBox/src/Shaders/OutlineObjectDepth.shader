#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
uniform mat4 u_Model;
uniform mat4 u_Scale;
uniform mat4 u_View;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_Model * u_View * u_Scale * vec4(positionA, 1);
}


#shader fragment
#version 330 core

void main()
{

}