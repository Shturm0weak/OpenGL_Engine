#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 btangent;

out vec2 TexCoords;

void main()
{
    TexCoords = textCoords;
    gl_Position = vec4(positions, 1);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Scene;
uniform sampler2D u_Blur;
uniform float u_Exposure;
uniform float u_Gamma;
uniform bool u_HasHDR;
uniform float u_Intensity;

void main()
{
    vec3 color = texture(u_Scene, TexCoords).rgb;
    vec3 blurColor = texture(u_Blur, TexCoords).rgb * u_Intensity;
    color += blurColor;
    vec3 result;
    if (u_HasHDR)
    {
        result = vec3(1.0) - exp(-color * u_Exposure);
        FragColor = vec4(pow(result, vec3(1.0 / u_Gamma)), 1.0);
    }
    else
    {
        FragColor = vec4(pow(color, vec3(1.0 / u_Gamma)), 1.0);
    }
}