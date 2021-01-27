#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 scale;

void main()
{
    gl_Position = proj * model * scale * vec4(pos.x, pos.y, pos.z, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = color;
}