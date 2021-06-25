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

uniform sampler2D scene;
uniform sampler2D bloomBlurH;
uniform float exposure;
uniform float gamma;
uniform bool hdr;
uniform float u_Intensity;

void main()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColorH = texture(bloomBlurH, TexCoords).rgb * u_Intensity;
    hdrColor += bloomColorH;
    vec3 result;
    if (hdr)
    {
        result = vec3(1.0) - exp(-hdrColor * exposure);
        //result = hdrColor / (hdrColor + vec3(1.0));
        FragColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0);
    }
    else
    {
        FragColor = vec4(pow(hdrColor, vec3(1.0 / gamma)), 1.0);
    }
}