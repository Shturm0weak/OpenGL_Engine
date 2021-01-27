#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;
layout(location = 1) in vec2 texcoords;
layout(location = 3) in vec4 m_color;
layout(location = 4) in float texIndex;
layout(location = 5) in vec4 rotationMat0;
layout(location = 6) in vec4 rotationMat1;
layout(location = 7) in vec4 rotationMat2;
layout(location = 8) in vec4 rotationMat3;
layout(location = 9) in vec2 posv;

out vec4 positionsfrag;
flat out int tex_index;
out vec4 out_color;
out vec2 v_textcoords;
uniform mat4 u_ViewProjection;
mat4 rot = mat4(
	rotationMat0,
	rotationMat1,
	rotationMat2,
	rotationMat3);

mat4 pos = mat4(1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				posv.x, posv.y, 0.0, 1.0);

void main() {
	gl_Position = u_ViewProjection * pos * rot * positions;
	v_textcoords = texcoords;
	out_color = m_color;
	tex_index = int(texIndex);
	positionsfrag = gl_Position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
in vec4 positionsfrag;
flat in int tex_index;
in vec4 out_color;
in vec2 v_textcoords;

uniform sampler2D u_Texture[32];
uniform float Brightness;

void main() {
	vec4 texColor = texture(u_Texture[tex_index], v_textcoords);
	FragColor = texColor * out_color;
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > Brightness)
		BrightColor = vec4(FragColor.rgb, 1.0);
	else
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
};