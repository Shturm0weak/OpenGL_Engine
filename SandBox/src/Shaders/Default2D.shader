#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 2) in int m_static;
layout(location = 3) in vec4 m_color;
layout(location = 4) in float texIndex;
layout(location = 5) in vec4 rotationMat0;
layout(location = 6) in vec4 rotationMat1;
layout(location = 7) in vec4 rotationMat2;
layout(location = 8) in vec4 rotationMat3;
layout(location = 10) in vec4 positionMat0;
layout(location = 11) in vec4 positionMat1;
layout(location = 12) in vec4 positionMat2;
layout(location = 13) in vec4 positionMat3;

out vec4 positionsfrag;
out float tex_index;
out vec4 out_color;
out vec2 v_textcoords;
uniform mat4 u_ViewProjection;
uniform mat4 u_Projection;
mat4 rot = mat4(
	rotationMat0,
	rotationMat1,
	rotationMat2,
	rotationMat3);

mat4 pos = mat4(
	positionMat0,
	positionMat1,
	positionMat2,
	positionMat3
);

void main() {
	if (m_static == 0) {
		gl_Position = u_ViewProjection * pos * rot * positions;
	}
	else if (m_static > 1) {
		gl_Position = u_Projection * pos * rot * positions;
	}

	v_textcoords = texcoords;
	out_color = m_color;
	tex_index = texIndex;
	positionsfrag = gl_Position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
in vec4 positionsfrag;
in float tex_index;
in vec4 out_color;
in vec2 v_textcoords;

uniform sampler2D u_Texture[32];

void main() {
	int index = int(tex_index);

	vec4 texColor = texture(u_Texture[index], v_textcoords);

	gl_FragColor = texColor * out_color;

};