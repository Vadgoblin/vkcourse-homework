#version 450

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_fragPos;

layout(push_constant) uniform PushConstants {
    vec3 cameraPosition;
    mat4 projection;
    mat4 view;
    mat4 model;
} constants;

layout(set = 0, binding = 0) uniform sampler2D gridImage;

layout(location = 0) out vec4 out_color;


const float ambientStrength = 0.1;
const float specularStrength = 0.5;
const float specularShininess = 32.0;

#define NUM_LIGHTS 3

struct Light {
    vec3 position;
    vec3 color;
};

layout(set = 1, binding = 0) uniform LightsUBO {
    Light lights[NUM_LIGHTS];
} ubo;

//const Light lights[NUM_LIGHTS] = Light[](
//    Light(vec3(0.0, 10.0, 10.0), vec3(1.0, 0.0, 0.0)),
//    Light(vec3(-5.0, 10.0, -10.0), vec3(0.0, 1.0, 0.0)),
//    Light(vec3(5.0, 10.0, -10.0), vec3(0.0, 0.0, 1.0))
//);



void main() {
    vec4 objectColor = texture(gridImage, in_uv);
    vec3 norm = normalize(in_normal);

    vec3 viewDir = normalize(constants.cameraPosition - in_fragPos);

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++){
        vec3 pos = ubo.lights[i].position;
        vec3 col = ubo.lights[i].color;

        // Diffuse
        vec3 lightDir = normalize(pos - in_fragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        // Specular (Phong)
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);

        // Accumulate raw light values
        totalDiffuse += diff * col;
        totalSpecular += spec * col * specularStrength;
    }

    // Combine results
    vec3 ambient = ambientStrength * vec3(1.0);

    // (Ambient + Diffuse) * Color + Specular
    // This separates texture color from specular highlights (so highlights stay white)
    vec3 result = (ambient + totalDiffuse) * objectColor.rgb + totalSpecular;


    out_color = vec4(result, 1.0);
}