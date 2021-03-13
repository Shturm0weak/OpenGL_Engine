#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 btangent;
layout(location = 5) in vec3 v;
layout(location = 6) in vec3 s;
layout(location = 7) in vec4 m_color;
layout(location = 8) in vec3 mat;
layout(location = 9) in mat4 u_View;
mat4 u_Model = mat4(1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					v.x, v.y, v.z, 1.0);
mat4 u_Scale = mat4(s.x, 0.0, 0.0, 0.0,
	0.0, s.y, 0.0, 0.0,
	0.0, 0.0, s.z, 0.0,
	0.0, 0.0, 0.0, 1.0);

uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * u_Model * u_View * u_Scale * vec4(positions, 1);
};


#shader fragment
#version 330 core

void main() {

};