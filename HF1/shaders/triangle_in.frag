#version 450

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D gridImage;

void main() {
//    out_color = vec4(in_color, 1.0f);

    vec4 pixel = texture(gridImage, in_uv);
    out_color = pixel;// * vec4(0.5, 0.5f, 0.5f, 0.5f);
}
