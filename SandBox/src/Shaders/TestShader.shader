#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 textCoords;

out vec2 textureCoords;

uniform mat4 model;
uniform mat4 proj;
uniform mat4 scale;

void main()
{
    gl_Position = proj * model * scale * vec4(pos.x, pos.y, pos.z, 1.0);
    textureCoords = textCoords;
}

#shader fragment
#version 330 core

in vec2 textureCoords;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D texture2;

void main()
{
    vec4 texColor = texture(texture2, textureCoords);
    FragColor = texColor * color;
}