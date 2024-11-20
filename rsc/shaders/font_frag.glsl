#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 color;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler[16];
layout(push_constant) uniform PushConstants {
    int textureIndex;
} pushConstants;


void main() {
    vec4 texColor = texture(texSampler[pushConstants.textureIndex], fragTexCoord);
    if (texColor.r < 0.01 && texColor.g < 0.01 && texColor.b < 0.01) {
        discard;
    }
    float intensity = texColor.r;
    outColor = vec4(color.rgb * intensity, texColor.a);
}