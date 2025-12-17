#version 450

layout(location = 0) in vec3 in_position;

layout(push_constant) uniform PushConstants {
    mat4 projection;
    mat4 view;
    mat4 model;
} constants;

void main() {
    vec3 current_pos = in_position;
    mat4 lightProjection = constants.projection;
    mat4 lightView = constants.view;

    gl_Position = lightProjection * lightView * constants.model * vec4(current_pos, 1.0f);
}
