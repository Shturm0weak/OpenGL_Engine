#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;

out float ambient;
out float specular;
out vec3 LightColor;
out vec3 LightPos;
out vec3 CameraPos;
out vec3 FragPos;
out vec3 Normal;
out vec4 out_color;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;
uniform vec4 m_color;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;
uniform float u_Ambient;
uniform float u_Specular;

void main() {
	FragPos = vec3(u_Model * u_Scale * vec4(positions, 1.0));
	gl_Position = u_ViewProjection * u_Model * u_View * u_Scale * vec4(positions,1);
	Normal = inverse(transpose(mat3(u_Model * u_View * u_Scale))) * normals;
	out_color = m_color;
	LightColor = (u_LightColor);
	LightPos = (u_LightPos);
	CameraPos = u_CameraPos;
	ambient = u_Ambient;
	specular = u_Specular;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
in vec3 CameraPos;
in vec3 LightColor;
in vec3 LightPos;
in vec3 FragPos;
in vec3 Normal;
in vec4 out_color;
in float ambient;
in float specular;

void main() {
	float ambientStrength = ambient;
	vec3 ambient = ambientStrength * LightColor;

	vec3 norm = normalize(Normal);
	vec3 LightDir = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(norm, LightDir), 0.0);
	vec3 diffuse = diffuseStrength * LightColor;

	float specularStrength = specular;
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 reflectDir = reflect(-LightDir,norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * LightColor;

	vec3 result = (ambient + diffuse + specular);

	gl_FragColor = vec4(result,1.0) * out_color;
};