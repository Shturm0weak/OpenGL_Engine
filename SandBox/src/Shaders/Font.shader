#shader vertex
#version 330 core

layout(location = 0) in vec4 positionA;
layout(location = 1) in vec2 uvA;
layout(location = 2) in int staticA;
layout(location = 3) in vec4 colorA;
layout(location = 4) in float isGuiA;
layout(location = 5) in float texIndexA;

flat out int texIndex;
out float isGui;
out vec4 color;
out vec2 uv;

uniform mat4 u_ViewProjection;
uniform mat4 u_Projection;

void main() {
	if (staticA == 0) {
		gl_Position = u_ViewProjection * positionA;
	}
	else if (staticA > 1) {
		gl_Position = u_Projection * positionA;
	}

	uv = uvA;
	color = colorA;
	isGui = isGuiA;
	texIndex = int(texIndexA);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;

flat in int texIndex;
in float isGui;
in vec4 color;
in vec2 uv;

uniform vec4 u_outlineColor;
uniform float u_width;
uniform float u_edge;
uniform float u_borderwidth;
uniform float u_borderedge;
uniform vec2 u_offset;
uniform sampler2D u_Texture[32];

void main() {
	if (isGui > 0.5) {
		vec4 texColor = texture(u_Texture[texIndex], uv);
		gl_FragColor = color * texColor;
	}
	else if (isGui <= 0.5) {
		float distance = 1.0 - texture(u_Texture[texIndex], uv).a;
		float alpha = 1.0 - smoothstep(u_width, u_width + u_edge, distance);
		float distance2 = 1.0 - texture(u_Texture[texIndex], uv + u_offset).a;
		float outlinealpha = 1.0 - smoothstep(u_borderwidth, u_borderwidth + u_borderedge, distance2);

		float overallalpha = alpha + (1.0 - alpha) * outlinealpha;
		vec3 overallcolor = mix(vec3(u_outlineColor.r, u_outlineColor.g, u_outlineColor.b), vec3(color.r, color.g, color.b), alpha / overallalpha);
		gl_FragColor = vec4(overallcolor, overallalpha);
	}
};