#include "LightManager.h"

#include <buffer.h>
#include <context.h>

// DescriptorMgmt descriptors;
// descriptors.SetDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
// descriptors.CreateLayout(device);
// descriptors.CreatePool(device);
//
// descriptors.CreateDescriptorSets(device, 1);
//
// struct UBO {
//     glm::vec4 colors[3];
// } ubo = {
//     {
//         {1.0f, 0.0f, 0.0f, 0.0f},
//         {0.0f, 1.0f, 0.0f, 0.0f},
//         {0.0f, 0.0f, 1.0f, 0.0f},
//     }
// };
//
// BufferInfo colorInfo = BufferInfo::Create(phyDevice, device, sizeof(ubo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
// colorInfo.Update(device, &ubo, sizeof(ubo));
//
// DescriptorSetMgmt &colorSet = descriptors.Set(0);
// colorSet.SetBuffer(0, colorInfo.buffer);
// colorSet.Update(device);

LightManager::LightManager(Context& context)
{
    m_lights[0].position = glm::vec3(0,10,0);
    m_lights[0].color = glm::vec3(1.0, 0.0, 0.0);

    m_lights[0].position = glm::vec3(-5,10,10);
    m_lights[0].color = glm::vec3(0.0, 1.0, 0.0);

    m_lights[0].position = glm::vec3(5,10,10);
    m_lights[0].color = glm::vec3(0.0, 0.0, 1.0);

    // m_pipelineLayout = context.pipelineWrapper().pipelineLayout();
    //
    // BufferInfo lightInfo = BufferInfo::Create(context.physicalDevice(), context.device(), sizeof(m_lights), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    // lightInfo.Update(context.device(), &m_lights, sizeof(m_lights));
    //
    //
    // VkDescriptorSetLayout descSetLayout = context.pipelineWrapper().descSetLayout();
    // m_modelSet = context.descriptorPool().CreateSet(descSetLayout);
    //
    // DescriptorSetMgmt setMgmt(m_modelSet);
    // setMgmt.SetBuffer(0,lightInfo.buffer);
    // setMgmt.Update(context.device());

    // auto device = context.device();
    // auto phyDevice = context.physicalDevice();
    //
    // DescriptorMgmt descriptors;
    // descriptors.SetDescriptor(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1);
    // descriptors.CreateLayout(device);
    // descriptors.CreatePool(device);
    //
    // descriptors.CreateDescriptorSets(device, 1);
    //
    //
    // BufferInfo colorInfo = BufferInfo::Create(phyDevice, device, sizeof(m_lights), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    // colorInfo.Update(device, &m_lights, sizeof(m_lights));
    //
    // DescriptorSetMgmt &setMgmt = descriptors.Set(0);
    // setMgmt.SetBuffer(0, colorInfo.buffer);
    // setMgmt.Update(device);
}

void LightManager::BindDescriptorSets(const VkCommandBuffer cmdBuffer)
{
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_modelSet, 0,
                            nullptr);
}