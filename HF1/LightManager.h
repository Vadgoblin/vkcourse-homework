#pragma once
#include <buffer.h>
// #include <context.h>
#include <glm/vec3.hpp>

#define NUM_LIGHTS 3
class Context;
class LightManager {
public:
    struct Light {
        glm::vec3 position; // 12 bytes
        float padding1;     // 4 bytes
        glm::vec3 color;    // 12 bytes
        float padding2;     // 4 bytes
    }; // Total: 32 bytes

    explicit LightManager(Context& context);

    void BindDescriptorSets(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) const;
    void Destroy(VkDevice device);

    VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_descSetLayout;}

private:
    Light m_lights[NUM_LIGHTS]{};
    // VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descSetLayout;
    VkDescriptorSet       m_descSet;
    BufferInfo            m_lightInfo;
};