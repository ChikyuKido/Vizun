#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler[16];
layout(push_constant) uniform PushConstants {
    int textureIndex;
} pushConstants;


void main() {
    vec4 texColor = texture(texSampler[pushConstants.textureIndex], fragTexCoord);
    if (texColor.a < 0.01) {
        discard;
    }
    outColor = texColor;
}