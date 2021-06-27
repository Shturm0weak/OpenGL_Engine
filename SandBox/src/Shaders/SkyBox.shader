#shader vertex
#version 330 core

layout(location = 0) in vec3 positionA;
layout(location = 1) in vec3 normalA;
layout(location = 2) in vec2 textureCoordsA;
layout(location = 4) in vec3 tangentA;
layout(location = 5) in vec3 btangentA;

out vec3 textureCoords;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Scale;
uniform mat4 u_ViewProjection;

void main() 
{
	textureCoords = vec3(u_Model * vec4(positionA, 1.0));
	vec4 pos = u_ViewProjection * u_View * u_Model * u_Scale * vec4(positionA, 1.0);
	gl_Position = pos.xyww;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform samplerCube u_DiffuseTexture;
uniform float u_Brightness;
in vec3 textureCoords;

void main() 
{
	FragColor = texture(u_DiffuseTexture, textureCoords);
	BrightColor = texture(u_DiffuseTexture, textureCoords);
	/*float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > Brightness)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);*/
}