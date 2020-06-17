#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;

uniform mat4 u_MVP;
uniform mat4 u_ViewProjection;

void main() {
	gl_Position = u_ViewProjection * u_MVP * vec4(positions, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 gl_FragColor;
float zNear = 0.1;
float zFar = 30.0;
in highp vec4 gl_FragCoord;
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / zFar;
	gl_FragColor = vec4(vec3(1 - depth,depth,depth),1.0);
};