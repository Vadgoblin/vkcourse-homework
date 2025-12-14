#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

layout(push_constant) uniform PushConstants {
    layout(offset = 4*4*4*0) mat4 projection;
    layout(offset = 4*4*4*1) mat4 view;
    layout(offset = 4*4*4*2) mat4 model;
} constants;

layout(location = 0) out vec2 out_uv;

void main() {
    vec3 current_pos = in_position;

    gl_Position = constants.projection
                 * constants.view
                 * constants.model
                 * vec4(current_pos, 1.0f);

    out_uv = in_uv;
}
