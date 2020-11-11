#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;

out vec2 blurtextcoords[11];
uniform mat4 u_ViewProjection;
uniform mat4 u_MVP;

void main() {
	vec2 cetertextcoords = (u_ViewProjection * u_MVP * positions) / 2 + 0.5;
	gl_Position = u_ViewProjection * u_MVP * positions;
	for (int i = -5; i < 5; i++)
	{
		blurtextcoords[i + 5] = cetertextcoords + vec2(0.3 * i,0.0);
	}
	
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 blurtextcoords[11];

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main() {
	vec4 texColor = vec4(0.0);
	texColor += texture(u_Texture, blurtextcoords[0]) * 0.0093;
	texColor += texture(u_Texture, blurtextcoords[1]) * 0.028002;
	texColor += texture(u_Texture, blurtextcoords[2]) * 0.065984;
	texColor += texture(u_Texture, blurtextcoords[3]) * 0.121703;
	texColor += texture(u_Texture, blurtextcoords[4]) * 0.175713;
	texColor += texture(u_Texture, blurtextcoords[5]) * 0.198596;
	texColor += texture(u_Texture, blurtextcoords[6]) * 0.175713;
	texColor += texture(u_Texture, blurtextcoords[7]) * 0.121703;
	texColor += texture(u_Texture, blurtextcoords[8]) * 0.065984;
	texColor += texture(u_Texture, blurtextcoords[9]) * 0.028002;
	texColor += texture(u_Texture, blurtextcoords[10]) * 0.0093;
};