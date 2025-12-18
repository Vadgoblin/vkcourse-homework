#pragma once
#include "../managers/LightManager.h"
#include "glm_config.h"
#include "texture.h"
#include <vulkan/vulkan_core.h>

#include <vector>

class Context;
class LightManager;
class ShadowPass {
public:
    ShadowPass(Context&              context,
               LightManager&         lightManager,
               VkFormat              depthFormat,
               VkExtent2D            extent);


    template<typename DrawFn>
    void DoPass(VkCommandBuffer cmdBuffer, DrawFn &&drawScene)
    {
        TransitionForRender(cmdBuffer);
        for (int i = 0; i < LightManager::NumberOfLights(); i++) {
            BeginNthPass(cmdBuffer,i);
            drawScene(cmdBuffer);
            EndNthPass(cmdBuffer);
        }
        TransitionForRead(cmdBuffer);
    }
    void Destroy(VkDevice device) const;

    VkExtent2D Extent() const { return m_extent; }
    uint32_t   Width() const { return m_extent.width; }
    uint32_t   Height() const { return m_extent.height; }

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline pipeline() const { return m_pipeline; }
    uint32_t modelPushConstantOffset () const { return m_modelPushConstantOffset; }
    VkDescriptorSetLayout ShadowMapDescSetLayout() const { return m_shadowMapDescSetLayout; }

    void BindDescriptorSets(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout);

private:
    struct LightInfoPushConstant {
        glm::mat4 projection;
        glm::mat4 view;
    };

    void TransitionForRender(VkCommandBuffer cmdBuffer);
    void TransitionForRead(VkCommandBuffer cmdBuffer);
    void BeginNthPass(VkCommandBuffer cmdBuffer, uint8_t n);
    void EndNthPass(VkCommandBuffer cmdBuffer);

    VkFormat m_depthFormat;// = VK_FORMAT_D32_SFLOAT_S8_UINT;
    LightManager& m_lightManager;

    VkExtent2D       m_extent            = {0, 0};
    VkPipelineLayout m_pipelineLayout    = VK_NULL_HANDLE;
    VkPipeline       m_pipeline          = VK_NULL_HANDLE;
    uint32_t         m_modelPushConstantOffset;
    VkDescriptorSetLayout m_shadowMapDescSetLayout;
    VkDescriptorSet m_shadowMapDescSet;
    std::vector<Texture*> m_shadowDepths;
};