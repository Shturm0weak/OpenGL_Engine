#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;

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

uniform sampler2D image;
uniform bool u_Horizontal;
uniform int u_PixelsAmount;
uniform int u_StepTexturePixles;
float weight[6] = float[](
    0.2578808493395437  ,
    0.20929982634420685 ,
    0.11188116791897133 ,
    0.03937394304056275 ,
    0.009116827159395895,
    0.001387810867091255);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * weight[0];
    if (u_Horizontal)
    {
        for (int i = 1; i < u_PixelsAmount; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i * u_StepTexturePixles, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i * u_StepTexturePixles, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < u_PixelsAmount; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i * u_StepTexturePixles)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i * u_StepTexturePixles)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}