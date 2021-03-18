#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
uniform mat4 u_Model;
uniform mat4 u_Scale;
uniform mat4 u_View;
uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * u_Model * u_View * u_Scale * vec4(positions, 1);
};


#shader fragment
#version 330 core

void main(){

};