#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) flat in uint texIndex;

layout(location = 0) out vec4 outColor;
layout(binding = 1) uniform sampler2D texSampler[16];

void main() {
    vec4 texColor = texture(texSampler[texIndex], fragTexCoord);
    if (texColor.a < 0.01) {
        discard;
    }
    outColor = texColor;
}