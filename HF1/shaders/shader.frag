#version 450

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_fragPos;
layout(location = 3) in vec3 camera_pos;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D gridImage;

const float ambientStrength = 0.0;
const float specularStrength = 0.5;
const float specularShininess = 32.0;

struct Light {
    vec3 position;
    vec3 color;
};

#define NUM_LIGHTS 3

const Light lights[NUM_LIGHTS] = Light[](
    Light(vec3(0.0, 10.0, 10.0), vec3(1.0, 0.0, 0.0)),
    Light(vec3(-5.0, 10.0, -10.0), vec3(0.0, 1.0, 0.0)),
    Light(vec3(5.0, 10.0, -10.0), vec3(0.0, 0.0, 1.0))
);



void main() {
    vec4 objectColor = texture(gridImage, in_uv);
    vec3 norm = normalize(in_normal);

    vec3 sum = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < NUM_LIGHTS; i++){
        vec3 pos = lights[i].position;
        vec3 col = lights[i].color;

        vec3 lightDir = normalize(pos - in_fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * col;

        vec3 viewDir = normalize(camera_pos - in_fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);
        vec3 specular = col * spec * specularStrength;


        sum += diffuse * objectColor.rgb + specular;
    }

    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    sum += ambient * objectColor.rgb;
    
    out_color = vec4(sum, 1.0);
}