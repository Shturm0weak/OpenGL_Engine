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

const vec3 outlineColor = vec3(1.0, 1.0, 1.0);

const float width = 0.9;
const float edge = 0.1;

const float borderwidth = 0.0;
const float borderedge = 0.0;

uniform sampler2D u_Texture[32];

void main() {
	//if (flagIsGui > 0.5) {
	//	int index = int(tex_index);
	//	vec4 texColor = texture(u_Texture[index], v_textcoords);
	//	color = texColor * out_color;
	//	//color = out_color;
	//}
	//else if(flagIsGui <= 0.5){
		//float distance = 1.0 - texture(u_Texture[tex_index], v_textcoords).a;
		//float alpha = 1.0 - smoothstep(width, width + edge, distance);
		int index = int(tex_index);
		vec4 texColor = texture(u_Texture[index], v_textcoords);
		color = texColor * out_color;
		//float distance2 = 1.0 - texture(u_Texture, v_textcoords).a;
		//float outlinealpha = 1.0 - smoothstep(borderwidth, borderwidth + borderedge, distance2);

		//float overallalpha = alpha + (1.0 - alpha) * outlinealpha;
		//vec3 overallcolor = mix(outlineColor, vec3(out_color.r, out_color.g, out_color.b), alpha / overallalpha);
		//color = vec4(out_color.r, out_color.g, out_color.b, alpha);
	//}
};