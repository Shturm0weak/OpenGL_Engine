#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in int m_static;
layout(location = 3) in vec4 m_color;
layout(location = 4) in float isGui;
layout(location = 5) in float texIndex;
layout(location = 6) in vec2 size;
layout(location = 7) in vec2 pos;
layout(location = 8) in float radius;
layout(location = 9) in vec2 windowSize;
layout(location = 10) in vec2 panelSize;
layout(location = 11) in vec2 panelPos;
layout(location = 12) in int relatedToPanel;

out float edgeRadius;
out vec2 uisize;
out vec2 uipos;
out float tex_index;
out float flagIsGui;
out vec4 out_color;
out vec2 v_textcoords;
out vec2 viewportsize;
out vec2 out_panelSize;
out vec2 out_panelPos;
out int out_relatedToPanel;

uniform mat4 u_ViewProjection;
uniform mat4 u_Projection;


void main() {
	if (m_static == 0) {
		gl_Position = u_ViewProjection * positions;
	}
	else if (m_static > 1) {
		gl_Position = u_Projection * positions;
	}

	v_textcoords = texcoords;
	out_color = m_color;
	flagIsGui = isGui;
	tex_index = texIndex;
	uisize = size;
	uipos = vec2(u_ViewProjection * vec4(pos,0,0));
	edgeRadius = radius;
	viewportsize = windowSize;
	out_panelSize = panelSize;
	out_panelPos = panelPos;
	out_relatedToPanel = relatedToPanel;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in float tex_index;
in float flagIsGui;
in vec4 out_color;
in vec2 v_textcoords;
in vec2 uisize;
in vec2 uipos;
in float edgeRadius;
in vec4 gl_FragCoord;
in vec2 gl_PointCoord;
in vec2 viewportsize;
in vec2 out_panelSize;
in vec2 out_panelPos;
in int out_relatedToPanel;

uniform vec4 u_outlineColor;
uniform float u_width;
uniform float u_edge;
uniform float u_borderwidth;
uniform float u_borderedge;
uniform vec2 u_offset;
uniform sampler2D u_Texture[32];

void RoundedCorners(vec4 texColor) {
	float alpha = 0.0;
	float ratio = (viewportsize.x / viewportsize.y);
	vec2 lower = vec2(uipos.x - uisize.x + edgeRadius, uipos.y + uisize.y + edgeRadius);
	vec2 higher = vec2(uipos.x + uisize.x - edgeRadius, uipos.y - uisize.y - edgeRadius);
	vec2 pP = gl_FragCoord.xy / viewportsize - 0.5;
	pP.x *= ratio;
	color = out_color * texColor;
	alpha = color.a;
	if (pP.x < lower.x && pP.x > lower.x - edgeRadius && pP.y < lower.y && pP.y > lower.y - edgeRadius) {
		if (length(pP - lower) > edgeRadius) {
			alpha = 0;
		}
	}
	if (pP.x > higher.x && pP.x < higher.x + edgeRadius && pP.y > higher.y && pP.y < higher.y + edgeRadius) {
		if (length(pP - higher) > edgeRadius) {
			alpha = 0;
		}
	}
	if (pP.x < lower.x && pP.x > lower.x - edgeRadius && pP.y < higher.y + edgeRadius && pP.y > higher.y) {
		if (length(pP - vec2(lower.x, higher.y)) > edgeRadius) {
			alpha = 0;
		}
	}
	if (pP.x > higher.x && pP.x < higher.x + edgeRadius && pP.y < lower.y && pP.y > lower.y - edgeRadius) {
		if (length(pP - vec2(higher.x, lower.y)) > edgeRadius) {
			alpha = 0;
		}
	}
	color.a = alpha;
}

void main() {
	if (flagIsGui > 0.5) {
		int index = int(tex_index);
		vec4 texColor = texture(u_Texture[index], v_textcoords);
		if (edgeRadius > 0) {
			RoundedCorners(texColor);
		}
		else {
			color = out_color * texColor;
		}
	}
	else if (flagIsGui <= 0.5) {
		int index = int(tex_index);
		float distance = 1.0 - texture(u_Texture[index], v_textcoords).a;
		float alpha = 1.0 - smoothstep(u_width, u_width + u_edge, distance);
		float distance2 = 1.0 - texture(u_Texture[index], v_textcoords + u_offset).a;
		float outlinealpha = 1.0 - smoothstep(u_borderwidth, u_borderwidth + u_borderedge, distance2);

		float overallalpha = alpha + (1.0 - alpha) * outlinealpha;
		vec3 overallcolor = mix(vec3(u_outlineColor.r, u_outlineColor.g, u_outlineColor.b), vec3(out_color.r, out_color.g, out_color.b), alpha / overallalpha);
		color = vec4(overallcolor, overallalpha);
	}
		
};