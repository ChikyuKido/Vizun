#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform PushConstants {
    layout(offset = 4) uint transformOffset;
} pushConstants;

layout(binding = 0) uniform UniformBufferObject {
    mat4 viewProj;
    mat4 view;
    mat4 proj;
} ubo;
layout(set = 0, binding = 2) readonly buffer StorageBuffer {
    mat4 transforms[];
} trans;

void main() {
    mat4 modelMatrix = trans.transforms[gl_InstanceIndex+pushConstants.transformOffset];
    gl_Position = ubo.viewProj * modelMatrix * vec4(inPosition, 0.0, 1.0);

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}