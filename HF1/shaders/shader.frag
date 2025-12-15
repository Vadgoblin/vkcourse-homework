#version 450

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_fragPos;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D gridImage;

const vec3 lightPos = vec3(10.0, 10.0, 10.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

const float ambientStrength = 0.1;

void main() {
    vec4 objectColor = texture(gridImage, in_uv);

    vec3 norm = normalize(in_normal);
    vec3 lightDir = normalize(lightPos - in_fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = ambientStrength * vec3(1.0, 0.0, 0.0);

    vec3 result = (ambient + diffuse) * objectColor.rgb;
    out_color = vec4(result, 1.0);
}