#include "LightManager.h"

#include <buffer.h>
#include <context.h>


LightManager::LightManager(Context& context)
{
    m_lights[0].position = glm::vec3(0,10,10);
    m_lights[0].color = glm::vec3(1.0, 0.0, 0.0);

    m_lights[1].position = glm::vec3(-5,10,-10);
    m_lights[1].color = glm::vec3(0.0, 1.0, 0.0);

    m_lights[2].position = glm::vec3(5,10,-10);
    m_lights[2].color = glm::vec3(0.0, 0.0, 1.0);

    m_lightInfo = BufferInfo::Create(context.physicalDevice(), context.device(), sizeof(m_lights), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    m_lightInfo.Update(context.device(), &m_lights, sizeof(m_lights));

    VkDescriptorSetLayoutBinding descSetLayoutBinding ={
        .binding            = 0,
        .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount    = 1,
        .stageFlags         = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr,
    };

    m_descSetLayout = context.descriptorPool().CreateLayout({descSetLayoutBinding});
    m_descSet       = context.descriptorPool().CreateSet(m_descSetLayout);

    DescriptorSetMgmt setMgmt(m_descSet);
    setMgmt.SetBuffer(0,m_lightInfo.buffer);
    setMgmt.Update(context.device());
}

void LightManager::BindDescriptorSets(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout) const
{
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &m_descSet, 0,nullptr);
}

void LightManager::Destroy(VkDevice device)
{
    m_lightInfo.Destroy(device);
}
