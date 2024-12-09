#version 450

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in uint inPackedData;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out uint texIndex;


layout(binding = 0) uniform UniformBufferObject {
    mat4 viewProj;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 2) readonly buffer StorageBuffer {
    mat4 transforms[];
} trans;

vec2[4] positions = vec2[4](
    vec2(0.0f, 0.0f),
    vec2(1.0f, 0.0f),
    vec2(1.0f, 1.0f),
    vec2(0.0f, 1.0f)
);
void main() {
    uint texIndex = inPackedData & 0x000000FF;
    uint posIndex = (inPackedData & 0x0000FF00)>>8;
    uint transformIndex = (inPackedData & 0xFFFF0000)>>16;
    mat4 modelMatrix = trans.transforms[transformIndex];
    gl_Position = ubo.viewProj * modelMatrix * vec4(positions[posIndex], 0.0, 1.0);
    texIndex = texIndex;
    fragTexCoord = inTexCoord;
}