#version 450

layout(location = 0) in vec2 in_uv;

layout(binding = 0) uniform sampler2D samplerColor;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform PushConstants {
    uint mode;
} constants;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 doLaplace() {
    vec4 result = vec4(0.0f);

    vec2 texelSize = 1.0 / textureSize(samplerColor, 0);

    mat3 laplace = mat3(
            0.0f, -1.0f, 0.0f,
            -1.0f, 4.0f, -1.0f,
            0.0f, -1.0f, 0.0f
        );

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec4 otherPixel = texture(samplerColor, in_uv + (vec2(x, y) * texelSize));
            result += laplace[x + 1][y + 1] * otherPixel;
        }
    }

    return result;
}

vec4 doBlur() {
    vec4 result = vec4(0.0f);

    vec2 texelSize = 1.0 / textureSize(samplerColor, 0);

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            vec4 otherPixel = texture(samplerColor, in_uv + (vec2(x, y) * texelSize));
            result += otherPixel;
        }
    }

    result /= 9.0f;
    return result;
}

vec4 doSepia() {
    vec4 pixel = texture(samplerColor, in_uv);

    vec4 sepia = vec4(112, 66, 20, 255) / 255.0f;

    return mix(pixel, sepia, 0.20f);
}

vec3 doMyShit(){
    vec4 pixel = texture(samplerColor, in_uv);

    if(pixel.r + pixel.g + pixel.g < 0.001) {
        float r = rand(in_uv * 1);
        pixel += vec4(r,r,r,1.0f)*0.075;
    }

    return pixel.rgb;
}

vec3 doMyShit2(){
    vec4 pixel = texture(samplerColor, in_uv);
    vec3 color = pixel.rgb;

    float dist = distance(in_uv, vec2(0.5));
    float vignette = 1.0 - smoothstep(0.0, 0.6, dist);
    vignette;

    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    vec3 bw = vec3(gray);

    vec3 out_color = vignette * pixel.rgb + (1-vignette) * bw;
    if(pixel.r + pixel.g + pixel.g < 0.001) {
        float r = rand(in_uv * 1);
        out_color += vec3(r,r,r)*0.05;
    }

    return out_color;
}

void main() {
    vec4 result = vec4(1.0);

    switch (constants.mode) {
        case 0:
        {
            vec4 pixel = texture(samplerColor, in_uv);
            result = pixel;
            break;
        }
        case 1:
        {
            vec4 pixel = texture(samplerColor, in_uv);
            result = mix(pixel, doLaplace(), 0.8f);
            break;
        }
        case 2:
        result = doBlur();
        break;
        case 3:
        result = doSepia();
        break;
        case 4:
        result = vec4(doMyShit().rgb,1.0f);
        break;
    }

    out_color = vec4(result.rgb, 1.0f);
}
