#version 450

#define NUM_LIGHTS 3

struct Light {
    vec3 position;
    vec3 color;
    mat4 projection;
    mat4 view;
};

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
layout(set = 1, binding = 0) uniform LightsUBO {
    Light lights[NUM_LIGHTS];
} ubo;

layout(set = 2, binding = 0) uniform sampler2D shadowMap[NUM_LIGHTS];

layout(location = 0) out vec4 out_color;

const float ambientStrength = 0.1;
const float specularStrength = 0.5;
const float specularShininess = 32.0;

float constant  = 1.0;
float linear    = 0.045;
float quadratic = 0.0075;


float SimpleShadow(vec4 fragPosLightSpace, int lightIndex) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform x-y to [0,1] range
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    // Check if current fragment is outside the shadow map bounds
    // If x, y, or z are outside [0, 1], force shadow to 0.0 (lit)
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0;
    }

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap[lightIndex], projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    if (projCoords.z > 1.0 || projCoords.z < -1.0f) {
        return 0.0f;
    }

    return currentDepth > closestDepth ? 1.0 : 0.0;
}

float PCFShadow(vec4 fragPosLightSpace, int lightIndex)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform x-y to [0,1] range
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap[lightIndex], projCoords.xy).r;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec3 normal = normalize(in_normal);
    vec3 lightDir = normalize(ubo.lights[lightIndex].position - in_fragPos);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(shadowMap[lightIndex], 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[lightIndex], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (projCoords.z > 1.0 || projCoords.z < -1.0f) {
        shadow = 0.0;
    }

    return shadow;
}



void main() {
    vec4 objectColor = texture(gridImage, in_uv);
    vec3 norm = normalize(in_normal);
    vec3 viewDir = normalize(constants.cameraPosition - in_fragPos);

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; i++){
        vec3 pos = ubo.lights[i].position;
        vec3 col = ubo.lights[i].color;

        vec3 lightDir = normalize(pos - in_fragPos);

        float distance = length(pos - in_fragPos);
        float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

        float diff = max(dot(norm, lightDir), 0.0);

        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);

        vec4 fragPosLightSpace = ubo.lights[i].projection * ubo.lights[i].view * vec4(in_fragPos, 1.0);

        float shadow = SimpleShadow(fragPosLightSpace, i);
//        float shadow = PCFShadow(fragPosLightSpace, i);

        totalDiffuse  += (1.0 - shadow) * (diff * col * attenuation);
        totalSpecular += (1.0 - shadow) * (spec * col * specularStrength * attenuation);
    }

    vec3 ambient = ambientStrength * vec3(1.0);

    vec3 result = (ambient + totalDiffuse) * objectColor.rgb + totalSpecular;

    out_color = vec4(result, 1.0);
}