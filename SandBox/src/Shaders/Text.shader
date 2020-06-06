#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in int m_static;
layout(location = 3) in vec4 m_color;
layout(location = 4) in float isGui;
layout(location = 5) in float texIndex;

out float tex_index;
out float flagIsGui;
out vec4 out_color;
out vec2 v_textcoords;
uniform mat4 u_ViewProjection;
uniform mat4 u_Projection;
uniform float u_ViewMatrix;


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
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in float tex_index;
in float flagIsGui;
in vec4 out_color;
in vec2 v_textcoords;

uniform vec4 u_outlineColor;
uniform float u_width;
uniform float u_edge;
uniform float u_borderwidth;
uniform float u_borderedge;
uniform vec2 u_offset;
uniform sampler2D u_Texture[32];

void main() {
	if (flagIsGui > 0.5) {
		int index = int(tex_index);
		vec4 texColor = texture(u_Texture[index], v_textcoords);
		color = texColor * out_color;
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