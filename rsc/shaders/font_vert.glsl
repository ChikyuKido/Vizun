#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 viewProj;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.viewProj * vec4(inPosition, 0.0, 1.0);
    fragTexCoord = inTexCoord;
}