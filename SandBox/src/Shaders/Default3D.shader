#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 textureCoordsA;
layout(location = 3) in vec3 vertexColorA;
layout(location = 4) in vec3 tangentA;
layout(location = 5) in vec3 btangentA;

out mat3 TBN;
out vec2 textureCoords;
out vec3 fragPos;
out vec3 inNormal;
out vec4 fragPosLightSpace;
out vec3 vertexColor;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;
uniform mat4 u_LightSpaceMatrix;

void main() 
{
	vec4 tempFragPos = u_Model * u_View * u_Scale * vec4(positionA, 1.0);
	fragPos = vec3(tempFragPos);
	fragPosLightSpace = u_LightSpaceMatrix * tempFragPos;
	mat3 modelVector = transpose(inverse(mat3(u_Model * u_View * u_Scale)));
	inNormal = normalize(modelVector * normalA);
	vec3 T = normalize(modelVector * tangentA);
	vec3 B = normalize(modelVector * btangentA);
	vec3 N = inNormal;
	TBN = (mat3(T, B, N));
	gl_Position = u_ViewProjection * tempFragPos;
	textureCoords = textureCoordsA;
	vertexColor = vertexColorA;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
in vec2 textureCoords;
in vec3 fragPos;
in vec3 inNormal;
in mat3 TBN;
in vec4 fragPosLightSpace;
in vec3 vertexColor;

struct PointLight {
	vec3 position;
	vec3 color;
	float constant;
	float _linear;
	float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 dir;
	vec3 color;
	float innerCutOff;
	float outerCutOff;
	float constant;
	float _linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 dir;
	vec3 color;
	float intensity;
};

#define MAX_LIGHT 32
uniform int pLightSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int dLightSize;
uniform DirectionalLight dirLights[MAX_LIGHT];
uniform int sLightSize;
uniform SpotLight spotLights[MAX_LIGHT];

uniform vec4 u_Color;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;
uniform float u_Ambient;
uniform float u_Specular;
uniform float u_Brightness;
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_ShadowTexture;
uniform sampler2D u_NormalMapTexture;
uniform bool u_IsNormalMapping;
uniform bool u_Emissive;
uniform bool u_DrawShadows;

vec3 normal = inNormal;
vec3 viewDir = normalize(u_CameraPos - fragPos);
vec3 diffuseTextureColor = texture(u_DiffuseTexture, textureCoords).rgb;
float shadow = 0.0;

float ShadowCalculation()
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	//float bias = 0.005;
	float bias = clamp(0.002 * tan(acos(dot(normal, dirLights[0].dir))), 0.0, 0.01);
	vec2 texelSize = 0.7 / textureSize(u_ShadowTexture, 0);
	int pcfRange = 4;
	for (int x = -pcfRange; x <= pcfRange; ++x)
	{
		for (int y = -pcfRange; y <= pcfRange; ++y)
		{
			float pcfDepth = texture(u_ShadowTexture, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= ((pcfRange * 2 + 1) * (pcfRange * 2 + 1));

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec3 DirLightsCompute(DirectionalLight light)
{
	vec3 ambient = u_Ambient * diffuseTextureColor * light.color;

	vec3 LightDir = normalize(light.dir);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTextureColor * diffuseStrength;

	vec3 reflectDir = reflect(-LightDir, normal);
	float specularf = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = u_Specular * specularf * diffuseTextureColor;

	return (ambient + (1.0 - shadow) * (diffuse + specular) * light.color) * light.intensity;
}

vec3 PointLightsCompute(PointLight light)
{
	vec3 ambient = u_Ambient * diffuseTextureColor * light.color;

	vec3 LightDir = normalize(light.position - fragPos);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTextureColor * diffuseStrength;

	vec3 reflectDir = reflect(-LightDir, normal);
	float specularf = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = u_Specular * specularf * diffuseTextureColor;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light._linear * distance +
		light.quadratic * (distance * distance));

	ambient *= attenuation;
	specular *= attenuation;
	diffuse *= attenuation;

	return (ambient + (1.0 - shadow) * (diffuse + specular) * light.color);
}

vec3 SpotLightsCompute(SpotLight light)
{
	vec3 LightDir = normalize(light.position - fragPos);
	float theta = dot(LightDir, normalize(-light.dir));

	if (theta > light.outerCutOff)
	{
		float epsilon = light.innerCutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambient = u_Ambient * diffuseTextureColor * light.color;

		float diffuseStrength = max(dot(normal, LightDir), 0.0);
		vec3 diffuse = diffuseTextureColor * diffuseStrength;

		vec3 reflectDir = reflect(-LightDir, normal);
		float specularf = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = u_Specular * specularf * diffuseTextureColor;

		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light._linear * distance +
			light.quadratic * (distance * distance));

		ambient  *= attenuation;
		specular *= attenuation;
		diffuse  *= attenuation;

		ambient  *= intensity;
		specular *= intensity;
		diffuse  *= intensity;

		return (ambient + (1.0 - shadow) * (diffuse + specular) * light.color);
	}
	else  // else, use ambient light so scene isn't completely dark outside the spotlight.
		return vec3(0.0, 0.0, 0.0);
}

void main()
{
	vec4 textureColor = texture(u_DiffuseTexture, textureCoords);
	if (textureColor.a < 0.05)
		discard;

	vec3 result = vec3(0, 0, 0);
	if (u_IsNormalMapping)
	{
		normal = texture(u_NormalMapTexture, textureCoords).rgb;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

	if (u_DrawShadows) {
		shadow = ShadowCalculation();
	}

	for (int i = 0; i < dLightSize; i++)
	{
		result += DirLightsCompute(dirLights[i]);
	}

	for (int i = 0; i < pLightSize; i++) 
	{
		result += PointLightsCompute(pointLights[i]);
	}  

	for (int i = 0; i < sLightSize; i++)
	{
		result += SpotLightsCompute(spotLights[i]);
	}

	FragColor = vec4(result * u_Color.rgb * vertexColor, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > u_Brightness || u_Emissive)
	{
		BrightColor = vec4(FragColor.rgb, 1.0);
	}
	else
	{
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}