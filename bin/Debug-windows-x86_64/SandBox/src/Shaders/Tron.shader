#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 btangent;
//layout(location = 5) in vec3 v;
//vec4 v1 = { v.x,0,0,0 };
//vec4 v2 = { 0,v.y,0,0 };
//vec4 v3 = { 0,0,v.z,0 };
//vec4 v4 = {0,0,0,0}
//mat4 model = { v1,v2,v3,4 };
out mat3 TBN;
out vec2 v_textcoords;
out float tempambient;
out float specular;
out vec3 LightColor;
out vec3 LightPos;
out vec3 CameraPos;
out vec3 FragPos;
out vec3 Normal;
out vec4 out_color;
out vec4 FragPosLightSpace;
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
uniform mat4 u_lightSpaceMatrix;

void main() {
	vec4 tempFragPos = u_Model * u_View * u_Scale * vec4(positions, 1.0);
	FragPos = vec3(tempFragPos);
	FragPosLightSpace = u_lightSpaceMatrix * tempFragPos;
	LightPos = (u_LightPos);
	CameraPos = u_CameraPos;
	out_color = m_color;
	LightColor = (u_LightColor);
	tempambient = u_Ambient;
	specular = u_Specular;
	v_textcoords = textCoords;
	mat3 modelVector = transpose(inverse(mat3(u_Model * u_View * u_Scale)));
	Normal = normalize(modelVector * normals);
	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * btangent);
	vec3 N = normalize(modelVector * normals);

	TBN = (mat3(T, B, N));

	gl_Position = u_ViewProjection * tempFragPos;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
out vec4 FragColor;
in vec2 v_textcoords;
in vec3 CameraPos;
in vec3 FragPos;
in vec3 Normal;
in vec4 out_color;
in float tempambient;
in float specular;
in mat3 TBN;
in vec4 FragPosLightSpace;
float ambient;

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
};


#define MAX_LIGHT 64
uniform int pLightSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int dLightSize;
uniform DirectionalLight dirLights[MAX_LIGHT];

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_ShadowTexture;
uniform sampler2D u_NormalMapTexture;
uniform bool u_isNormalMapping;
uniform float u_DrawShadows;

uniform float u_EdgeThickness;
uniform float u_EdgeSharpness;
uniform float u_EdgeSubstruct;
uniform float u_GlowStrength;

float shadow = 0.0;

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

	vec2 texelSize = 1.0 / textureSize(u_ShadowTexture, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_ShadowTexture, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec3 DirLightsCompute(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 CameraPos) {
	vec3 diffuseTexColor = texture(u_DiffuseTexture, v_textcoords).rgb;
	float ambientStrength = ambient;
	vec3 ambient = ambientStrength * diffuseTexColor * light.color;

	vec3 LightDir = normalize(light.dir);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTexColor * diffuseStrength;

	float specularStrength = specular;
	vec3 viewDir = normalize(CameraPos - FragPos);
	vec3 reflectDir = reflect(-LightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * diffuseTexColor;

	return (ambient + (1.0 - shadow) * (diffuse + specular) * light.color);
}

vec3 PointLightsCompute(PointLight light, vec3 normal, vec3 fragPos, vec3 CameraPos) {
	vec3 diffuseTexColor = texture(u_DiffuseTexture, v_textcoords).rgb;
	float ambientStrength = ambient;
	vec3 ambient = ambientStrength * diffuseTexColor * light.color;

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

	ambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (ambient + (diffuse + specular) * light.color);
}

void main() {
	if (u_DrawShadows > 0.5) {
		shadow = ShadowCalculation(FragPosLightSpace);
	}
	ambient = tempambient;
	vec3 normal = (Normal);
	vec3 result = vec3(0, 0, 0);
	if (u_isNormalMapping) {
		normal = texture(u_NormalMapTexture, v_textcoords).rgb;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN *  normal);
	}

	for (int i = 0; i < dLightSize; i++)
	{
		result += DirLightsCompute(dirLights[i], normal, FragPos, CameraPos);
	}

	for (int i = 0; i < pLightSize; i++) {
		result += PointLightsCompute(pointLights[i], normal, FragPos, CameraPos);
	}

	vec2 uv = abs(v_textcoords - 0.5) * u_EdgeThickness;

	uv = pow(uv,vec2(u_EdgeSharpness, u_EdgeSharpness)) - u_EdgeSubstruct;

	float c = clamp(uv.y + uv.x,0.01,1.0) * u_GlowStrength;

	FragColor = vec4(result * (c),1.0) * out_color;
};