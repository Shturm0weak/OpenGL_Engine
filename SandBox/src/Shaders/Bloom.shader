#shader vertex
#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 btangent;

out vec2 TexCoords;

void main() {
    TexCoords = textCoords;
    gl_Position =  vec4(positions, 1);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlurH;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColorH = texture(bloomBlurH, TexCoords).rgb;
    hdrColor += (bloomColorH); // additive blending
    // tone mapping
    vec3 result;
    if (bloomColorH.x > 0.1 || bloomColorH.y > 0.1) {
        result = vec3(1.0) - exp(-hdrColor * exposure);
    }
    else {
        result = hdrColor;
    }
    // also gamma correct while we're at it
    FragColor = vec4(result, 1.0);
}