#pragma once
#include <context.h>
#include <glm/vec3.hpp>

#define NUM_LIGHTS 3

class LightManager {
public:
    struct Light {
        glm::vec3 position; // 12 bytes
        float padding1;     // 4 bytes
        glm::vec3 color;    // 12 bytes
        float padding2;     // 4 bytes
    }; // Total: 32 bytes

    LightManager(Context& context);
    void BindDescriptorSets(const VkCommandBuffer cmdBuffer);

private:
    Light m_lights[NUM_LIGHTS]{};
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSet       m_modelSet      = VK_NULL_HANDLE;
};