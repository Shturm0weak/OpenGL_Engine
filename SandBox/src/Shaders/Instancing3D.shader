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
layout(location = 8) in vec2 mat;
layout(location = 9) in mat4 u_View;
mat4 u_Model = mat4(1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					v.x, v.y, v.z, 1.0);
mat4 u_Scale = mat4(s.x, 0.0, 0.0, 0.0,
					0.0, s.y, 0.0, 0.0,
					0.0, 0.0, s.z, 0.0,
					0.0, 0.0, 0.0, 1.0);
out mat3 TBN;
out vec2 v_textcoords;
out float ambient;
out float specular;
out vec3 LightColor;
out vec3 LightPos;
out vec3 CameraPos;
out vec3 FragPos;
out vec3 Normal;
out vec4 out_color;
uniform mat4 u_ViewProjection;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;

void main() {
	FragPos =  vec3(u_Model * u_Scale * vec4(positions, 1.0));
	LightPos =  (u_LightPos);
	CameraPos =  u_CameraPos;
	out_color = m_color;
	LightColor = (u_LightColor);
	ambient = mat.x;
	specular = mat.y;
	v_textcoords = textCoords;
	mat3 modelVector = transpose(inverse(mat3(u_Model * u_View * u_Scale)));
	Normal = normalize(modelVector * normals);
	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * btangent);
	vec3 N = normalize(modelVector * normals);

	TBN = (mat3(T, B, N));

	gl_Position = u_ViewProjection * u_Model * u_View * u_Scale * vec4(positions, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
in vec2 v_textcoords;
in vec3 CameraPos;
in vec3 LightColor;
in vec3 LightPos;
in vec3 FragPos;
in vec3 Normal;
in vec4 out_color;
in float ambient;
in float specular;
in mat3 TBN;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalMapTexture;
uniform bool u_isNormalMapping;

void main() {

	vec3 normal = (Normal);

	if (u_isNormalMapping) {
		normal = texture(u_NormalMapTexture, v_textcoords).rgb;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN *  normal);
	}

	vec3 diffuseTexColor = texture(u_DiffuseTexture, v_textcoords).rgb;
	float ambientStrength = ambient;
	vec3 ambient = ambientStrength * diffuseTexColor;

	vec3 LightDir = normalize(LightPos - FragPos);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTexColor * diffuseStrength;

	float specularStrength = specular;
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 reflectDir = reflect(-LightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * diffuseTexColor;

	vec3 result = (ambient + diffuse + specular);

	gl_FragColor = vec4(result, 1.0) * out_color * vec4(LightColor,1.0);
};