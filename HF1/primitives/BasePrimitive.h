#pragma once

#include "IDrawable.h"
#include "ITransformable.h"

#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "glm/fwd.hpp"
#include "glm_config.h"

#include <context.h>
#include <string.h>
#include <vector>

class ShadowPass;
class LightningPass;
class Context;

class BasePrimitive : public ITransformable, public IDrawable {
public:
    static VkDescriptorSetLayout CreateVertexDataDescSetLayout(Context& context)
    {
        VkDescriptorSetLayoutBinding descSetLayoutBinding{
            .binding            = 0,
            .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount    = 1,
            .stageFlags         = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        };

        return context.descriptorPool().CreateLayout({descSetLayoutBinding});
    }

    struct ModelPushConstant {
        glm::mat4 model;
    };

    BasePrimitive() {}
    ~BasePrimitive() override = default;

    VkResult create(Context&       context,
                    LightningPass& lightningPass,
                    ShadowPass&    shadowPass,
                    const char*    texture_name = "default");
    void     destroy(VkDevice device);
    void draw(VkCommandBuffer cmdBuffer, bool lightningPass, const glm::mat4& parentModel = glm::mat4(1.0f)) override;

protected:
    VkPipelineLayout m_lightningPassPipelineLayout;
    VkPipeline       m_lightningPassPipeline;
    uint32_t         m_lightningPassConstantOffset;

    VkPipelineLayout m_shadowPassPipelineLayout;
    VkPipeline       m_shadowPassPipeline;
    uint32_t         m_shadowPassConstantOffset;

    BufferInfo m_vertexBuffer   = {};
    BufferInfo m_indexBuffer    = {};
    BufferInfo m_texCoordBuffer = {};
    BufferInfo m_normalBuffer   = {};

    std::vector<float>        m_vertices;
    std::vector<float>        m_normals;
    std::vector<float>        m_texCoords;
    std::vector<unsigned int> m_indices;

    uint32_t m_vertexCount;

    VkDescriptorSet m_modelSet;

private:
    template <typename T>
    static BufferInfo
    UploadToGPU(const Context& context, const std::vector<T>& data, const VkBufferUsageFlagBits usageBits)
    {
        const uint32_t dataSize = data.size() * sizeof(T);

        BufferInfo buffer_info = BufferInfo::Create(context.physicalDevice(), context.device(), dataSize,
                                                    usageBits | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

        void* dataPtr = buffer_info.Map(context.device());
        memcpy(dataPtr, data.data(), dataSize);
        buffer_info.Unmap(context.device());

        return buffer_info;
    }
};