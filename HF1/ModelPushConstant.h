#pragma once
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ModelPushConstant {
    glm::mat4 model;
};