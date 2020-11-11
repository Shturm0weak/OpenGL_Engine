#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 btangent;
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