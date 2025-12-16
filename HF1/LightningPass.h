#pragma once
// #include "TextureManager.h"
#include "LightManager.h"
#include "glm_config.h"
#include <vulkan/vulkan_core.h>

#include <vector>

class Context;
class TextureManager;

class LightningPass {
public:
    LightningPass(Context &context,
                  TextureManager &textureManager,
                  LightManager &lightManager,
                  VkFormat colorFormat);

    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         modelPushConstantOffset() const { return m_modelPushConstantOffset; }
    TextureManager& textureManager() const { return m_textureManager;}

private:
    VkDevice              m_vkDevice;
    VkPipelineLayout      m_pipelineLayout;
    VkPipeline            m_pipeline;
    glm::uint32_t         m_modelPushConstantOffset;
    VkDescriptorSetLayout m_vertexDataDescSetLayout;

    TextureManager& m_textureManager;
    LightManager&    m_lightManager;
};
