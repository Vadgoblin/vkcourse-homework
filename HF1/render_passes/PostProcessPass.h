#pragma once

#include <vulkan/vulkan_core.h>

#include "context.h"
#include "texture.h"

#include <swapchain.h>

class PostProcessPass {
public:
    struct PostProcessOptions {
        uint32_t mode = 4;
    } options;

    PostProcessPass(VkFormat colorFormat, VkExtent2D extent);

    bool Create(Context& context);
    void Destroy(Context& context);

    template <typename DrawFn> void DoPass(VkCommandBuffer cmdBuffer,const Swapchain::Image  &img,DrawFn&& postPostprocessDraws)
    {
        TransitionForRender(cmdBuffer,img.image);
        BeginPass(cmdBuffer,img.view);
        Draw(cmdBuffer);
        postPostprocessDraws(cmdBuffer);
        EndPass(cmdBuffer);
        TransitionForRead(cmdBuffer, img.image);
    }

    void BindInputImage(VkDevice device, const Texture& texture);

    VkPipeline       Pipeline() const { return m_pipeline; }
    VkPipelineLayout PipelineLayout() const { return m_pipelineLayout; }

private:
    void BeginPass(VkCommandBuffer cmdBuffer, VkImageView colorOutputView);
    void Draw(VkCommandBuffer cmdBuffer);
    void EndPass(VkCommandBuffer cmdBuffer);
    void TransitionForRender(VkCommandBuffer cmdBuffer, VkImage vk_image) const;
    void TransitionForRead(VkCommandBuffer cmdBuffer, VkImage vk_image) const;

    VkFormat   m_colorFormat = {};
    VkExtent2D m_extent      = {};

    VkDescriptorSet  m_descSet        = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline       m_pipeline       = VK_NULL_HANDLE;
};
