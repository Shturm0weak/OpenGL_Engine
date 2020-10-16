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
out vec4 FragPosLightSpace;
out mat3 TBN;
out vec2 v_textcoords;
out float ambient;
out float specular;
out vec3 CameraPos;
out vec3 FragPos;
out vec3 Normal;
out vec4 out_color;
uniform mat4 u_ViewProjection;
uniform vec3 u_CameraPos;
uniform mat4 u_LightSpaceMatrix;

void main() {
	FragPos =  vec3(u_Model * u_Scale * vec4(positions, 1.0));
	FragPosLightSpace = u_LightSpaceMatrix * u_Model * u_View * u_Scale * vec4(positions, 1.0);
	CameraPos =  u_CameraPos;
	out_color = m_color;
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
in vec3 FragPos;
in vec3 Normal;
in vec4 out_color;
in float ambient;
in float specular;
in mat3 TBN;
in vec4 FragPosLightSpace;

struct PointLight {
	vec3 position;
	vec3 color;
	float constant;
	float _linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 dir;
	vec3 color;
	float intensity;
};


#define MAX_LIGHT 64
uniform int pLightSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int dLightSize;
uniform DirectionalLight dirLights[MAX_LIGHT];

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_NormalMapTexture;
uniform bool u_isNormalMapping;
uniform sampler2D u_ShadowMap;
uniform float u_DrawShadows;

float shadow = 0.0;

vec3 DirLightsCompute(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 CameraPos) {
	vec3 diffuseTexColor = texture(u_DiffuseTexture, v_textcoords).rgb;
	float ambientStrength = ambient * light.intensity;
	vec3 dambient = ambientStrength * diffuseTexColor;

	vec3 LightDir = normalize(light.dir);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTexColor * diffuseStrength;

	float specularStrength = specular;
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 reflectDir = reflect(-LightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * diffuseTexColor;

	return (dambient + (1.0 - shadow) * (diffuse + specular) * light.color);
}

vec3 PointLightsCompute(PointLight light, vec3 normal, vec3 fragPos, vec3 CameraPos) {
	vec3 diffuseTexColor = texture(u_DiffuseTexture, v_textcoords).rgb;
	float ambientStrength = ambient;
	vec3 pambient = ambientStrength * diffuseTexColor * light.color;

	vec3 LightDir = normalize(light.position - FragPos);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTexColor * diffuseStrength;

	float specularStrength = specular;
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 reflectDir = reflect(-LightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * diffuseTexColor;

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light._linear * distance +
		light.quadratic * (distance * distance));

	pambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (pambient + (diffuse + specular) * light.color);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float bias = 0.005;

	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main() {
	if (u_DrawShadows > 0.5) {
		shadow = ShadowCalculation(FragPosLightSpace);
	}
	vec3 normal = (Normal);
	vec3 result = vec3(0,0,0);
	if (u_isNormalMapping) {
		normal = texture(u_NormalMapTexture, v_textcoords).rgb;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN *  normal);
	}

	for (int i = 0; i < dLightSize; i++)
	{
		result += DirLightsCompute(dirLights[i], normal, FragPos, CameraPos);
	}

	for (int i = 0; i < pLightSize; i++){
		result += PointLightsCompute(pointLights[i], normal, FragPos, CameraPos);
	}

	gl_FragColor = vec4(result, 1.0) * out_color;
};