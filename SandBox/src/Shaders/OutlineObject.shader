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
uniform sampler2D u_OutlineDepth;
uniform bool u_HasOutline;

void main()
{
    vec3 result = texture(u_Scene, TexCoords).rgb;

    int w = 2;

    if (u_HasOutline && texture(u_OutlineDepth, TexCoords).xyz != vec3(1.0f))
    {
        vec2 size = 1.0f / textureSize(u_OutlineDepth, 0);

        for (int i = -w; i <= +w; i++)
        {
            for (int j = -w; j <= +w; j++)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }

                vec2 offset = vec2(i, j) * size;

                if (texture(u_OutlineDepth, TexCoords + offset).xyz == vec3(1.0f))
                {
                    FragColor = vec4(1.0, 1.0, 0.0, 1.0f);
                    return;
                }
            }
        }
    }
    FragColor = vec4(result, 1.0);
}