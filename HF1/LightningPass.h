#pragma once
#include "LightManager.h"
#include "glm_config.h"
#include "texture.h"
#include <vulkan/vulkan_core.h>

class ShadowPass;
class Context;
class TextureManager;

class LightningPass {
public:
    LightningPass(Context&              context,
                  TextureManager&       textureManager,
                  LightManager&         lightManager,
                  ShadowPass&           shadowPass,
                  VkFormat              colorFormat,
                  VkSampleCountFlagBits msaaLevel,
                  VkFormat              depthFormat,
                  VkExtent2D            extent);

    void BeginPass(VkCommandBuffer cmdBuffer);
    void EndPass(VkCommandBuffer cmdBuffer) const;
    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         modelPushConstantOffset() const { return m_modelPushConstantOffset; }
    TextureManager&  textureManager() const { return m_textureManager; }

    Texture& colorOutput() const { return *m_colorOutput; }

private:
    void TransitionForRender(VkCommandBuffer cmdBuffer) const;
    void TransitionForRead(VkCommandBuffer cmdBuffer) const;

    VkDevice              m_device;
    VkPhysicalDevice      m_phyDevice;
    VkPipelineLayout      m_pipelineLayout;
    VkPipeline            m_pipeline;
    glm::uint32_t         m_modelPushConstantOffset;
    // VkDescriptorSetLayout m_vertexDataDescSetLayout;

    VkFormat              m_colorFormat;
    VkFormat              m_depthFormat;
    VkExtent2D            m_extent;
    VkSampleCountFlagBits m_sampleCountFlagBits;

    Texture* m_colorOutput;
    Texture* m_colorOutputMsaa;
    Texture* m_depthOutput;
    Texture* m_depthOutputMsaa;

    TextureManager& m_textureManager;
    LightManager&   m_lightManager;
    ShadowPass&     m_shadowPass;
};
