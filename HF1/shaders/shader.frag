#version 450

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_fragPos;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D gridImage;

// Hardcoded light for testing (White light at 10, 10, 10)
// Later, pass this via Uniform Buffer!
const vec3 lightPos = vec3(10.0, 10.0, 10.0);
const vec3 lightColor = vec3(0.0, 1.0, 1.0);
const float ambientStrength = 0.1;

void main() {
    // 0. Sample the texture (The object's base color)
    vec4 objectColor = texture(gridImage, in_uv);

    // 1. Normalize Inputs
    // Interpolation across the triangle can make the normal length < 1.0, so re-normalize.
    vec3 norm = normalize(in_normal);

    // 2. Calculate Light Direction
    // Vector pointing FROM the surface TO the light
    vec3 lightDir = normalize(lightPos - in_fragPos);

    // 3. Diffuse Lighting (The "Angle" Math)
    // dot product > 0 means the face is looking AT the light.
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 4. Ambient Lighting (The "Base" Brightness)
    vec3 ambient = ambientStrength * lightColor;

    // 5. Combine results
    // (Ambient + Diffuse) * ObjectColor
    vec3 result = (ambient + diffuse) * objectColor.rgb;

    out_color = vec4(result, 1.0);
}


//void main() {
//    vec4 pixel = texture(gridImage, in_uv);
//    out_color = pixel;
//}
