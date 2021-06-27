#shader vertex
#version 330 core

layout(location = 0) in vec4 positionA;
layout(location = 1) in vec2 textureCoordsA;
layout(location = 3) in vec4 colorA;
layout(location = 4) in float textureIndexA;
layout(location = 5) in vec4 rotationMat0;
layout(location = 6) in vec4 rotationMat1;
layout(location = 7) in vec4 rotationMat2;
layout(location = 8) in vec4 rotationMat3;
layout(location = 9) in vec3 v; //position
layout(location = 10) in float emissiveA;

flat out int emissive;
flat out int textureIndex;
out vec4 color;
out vec2 textureCoords;
uniform mat4 u_ViewProjection;
mat4 rot = mat4(
	rotationMat0,
	rotationMat1,
	rotationMat2,
	rotationMat3);

mat4 pos = mat4(1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				v.x, v.y, v.z, 1.0);

void main() 
{
	gl_Position = u_ViewProjection * pos * rot * positionA;
	textureCoords = textureCoordsA;
	color = colorA;
	textureIndex = int(textureIndexA);
	emissive = int(emissiveA);
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
flat in int textureIndex;
flat in int emissive;
in vec4 color;
in vec2 textureCoords;

uniform sampler2D u_Texture[32];
uniform float u_Brightness;

void main()
{
	vec4 textureColor = texture(u_Texture[textureIndex], textureCoords);
	FragColor = textureColor * color;
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > u_Brightness || emissive == 1)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}