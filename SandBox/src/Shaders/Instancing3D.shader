#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 textureCoordsA;
layout(location = 3) in vec3 vertexColorA;
layout(location = 4) in vec3 tangentA;
layout(location = 5) in vec3 btangentA;
layout(location = 6) in vec3 v; //position
layout(location = 7) in vec3 s; //scale
layout(location = 8) in vec4 colorA;
layout(location = 9) in vec4 matA;
layout(location = 10) in mat4 viewA;

mat4 u_Model = mat4(1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					v.x, v.y, v.z, 1.0);
mat4 u_Scale = mat4(s.x, 0.0, 0.0, 0.0,
					0.0, s.y, 0.0, 0.0,
					0.0, 0.0, s.z, 0.0,
					0.0, 0.0, 0.0, 1.0);

flat out int emissive;
flat out int textureIndex;
out vec4 fragPosLightSpace;
out mat3 TBN;
out vec3 vertexColor;
out vec2 textureCoords;
out float ambientf;
out float specularf;
out vec3 fragPos;
out vec3 inNormal;
out vec4 color;

uniform mat4 u_ViewProjection;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	emissive = int(matA.w);
	textureIndex = int(matA.z);
	ambientf = matA.x;
	specularf = matA.y;
	vertexColor = vertexColorA;
	color = colorA;
	textureCoords = textureCoordsA;
	vec4 tempFragPos = u_Model * viewA * u_Scale * vec4(positionA, 1.0);
	fragPos = vec3(tempFragPos);
	fragPosLightSpace = u_LightSpaceMatrix * tempFragPos;
	mat3 modelVector = transpose(inverse(mat3(u_Model * viewA * u_Scale)));
	inNormal = normalize(modelVector * normalA);
	vec3 T = normalize(modelVector * tangentA);
	vec3 B = normalize(modelVector * btangentA);
	vec3 N = inNormal;
	TBN = (mat3(T, B, N));
	gl_Position = u_ViewProjection * tempFragPos;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

flat in int emissive;
in vec2 textureCoords;
in vec3 fragPos;
in vec3 inNormal;
in vec4 color;
in float ambientf;
in float specularf;
in mat3 TBN;
in vec4 fragPosLightSpace;
in vec3 vertexColor;
flat in int textureIndex;

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
uniform int sLightSize;
uniform SpotLight spotLights[MAX_LIGHT];
uniform int pLightSize;
uniform PointLight pointLights[MAX_LIGHT];
uniform int dLightSize;
uniform DirectionalLight dirLights[MAX_LIGHT];

uniform bool u_IsNormalMapping;
uniform bool u_DrawShadows;
uniform sampler2D u_Texture[32];
uniform float u_Brightness;
uniform vec3 u_CameraPos;
uniform int u_PcfRange;
uniform float u_ScalarTexelSize;
uniform float u_ShadowBias;

vec3 normal = inNormal;
vec3 diffuseTextureColor = texture(u_Texture[textureIndex], textureCoords).rgb;
vec3 viewDir = normalize(u_CameraPos - fragPos);
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
	float bias = clamp(u_ShadowBias * tan(acos(dot(normal, dirLights[0].dir))), 0.0, 0.01);
	vec2 texelSize = u_ScalarTexelSize / textureSize(u_Texture[2], 0);
	for (int x = -u_PcfRange; x <= u_PcfRange; ++x)
	{
		for (int y = -u_PcfRange; y <= u_PcfRange; ++y)
		{
			float pcfDepth = texture(u_Texture[2], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= ((u_PcfRange * 2 + 1) * (u_PcfRange * 2 + 1));

	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

vec3 DirLightsCompute(DirectionalLight light) 
{
	vec3 ambient = ambientf * diffuseTextureColor * light.color;

	vec3 LightDir = normalize(light.dir);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTextureColor * diffuseStrength;

	vec3 reflectDir = reflect(-LightDir, normal);
	float specular1f = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularf * specular1f * diffuseTextureColor;

	return (ambient + (1.0 - shadow) * (diffuse + specular) * light.color) * light.intensity;
}

vec3 PointLightsCompute(PointLight light) 
{
	vec3 ambient = ambientf  * diffuseTextureColor * light.color;

	vec3 LightDir = normalize(light.position - fragPos);
	float diffuseStrength = max(dot(normal, LightDir), 0.0);
	vec3 diffuse = diffuseTextureColor * diffuseStrength;

	vec3 reflectDir = reflect(-LightDir, normal);
	float specular1f = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularf * specular1f * diffuseTextureColor;

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
	
		vec3 ambient = ambientf * diffuseTextureColor * light.color;
	
		float diffuseStrength = max(dot(normal, LightDir), 0.0);
		vec3 diffuse = diffuseTextureColor * diffuseStrength;
	
		vec3 reflectDir = reflect(-LightDir, normal);
		float specular1f = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularf * specular1f * diffuseTextureColor;
	
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
	vec4 textureColor = texture(u_Texture[textureIndex], textureCoords);
	if (textureColor.a < 0.1)
		discard;

	vec3 result = vec3(0,0,0);
	if (u_IsNormalMapping) 
	{
		normal = texture(u_Texture[1], textureCoords).rgb;
		normal *= normal * 2.0 - 1.0;
		normal = normalize(TBN *  normal);
	}
	if (u_DrawShadows) 
	{
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

	FragColor = vec4(result * color.rgb * vertexColor, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > u_Brightness || emissive == 1)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}