#include "LightManager.h"

#include <buffer.h>
#include <context.h>


LightManager::LightManager(Context& context) : m_device(context.device())
{
    float lightFov = 40;

    m_lights[0].position = glm::vec3(10000,-10000,10000);
    m_lights[0].color = glm::vec3(1.5, 0.0, 0.0);
    m_lights[0].projection = glm::perspective(glm::radians(lightFov), 1.0f, 0.1f, 100.0f);


    m_lights[1].position = glm::vec3(10000,-10000,10000);
    m_lights[1].color = glm::vec3(0.0, 1.5, 0.0);
    m_lights[1].projection = glm::perspective(glm::radians(lightFov), 1.0f, 0.1f, 100.0f);

    m_lights[2].position = glm::vec3(10000,-10000,10000);
    m_lights[2].color = glm::vec3(0.0, 0.0, 1.5);
    m_lights[2].projection = glm::perspective(glm::radians(lightFov), 1.0f, 0.1f, 100.0f);



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

void LightManager::Destroy()
{
    m_lightInfo.Destroy(m_device);
}

void LightManager::Tick(float amount)
{
    m_animationProgress += amount;
    while (m_animationProgress >= 360.0f) m_animationProgress -= 360.0f;
    SetPosition();
}

void LightManager::SetPosition()
{
    float s = 20.0f;
    float angleDeg = m_animationProgress;
    float angleRad = angleDeg * M_PI / 180.0f;

    float h = s * std::sqrt(3.0f) / 2.0f;
    float c = std::cos(angleRad);
    float si = std::sin(angleRad);

    glm::vec2 A(-s/2, -h/3);
    glm::vec2 B( s/2, -h/3);
    glm::vec2 C( 0.0f, 2*h/3);

    A = { A.x * c - A.y * si,
          A.x * si + A.y * c};

    B = { B.x * c - B.y * si,
          B.x * si + B.y * c};

    C = { C.x * c - C.y * si,
          C.x * si + C.y * c };

    m_lights[0].position = glm::vec3(A.x, 10.0f, A.y);
    m_lights[1].position = glm::vec3(B.x, 10.0f, B.y);
    m_lights[2].position = glm::vec3(C.x, 10.0f, C.y);

    for (auto & m_light : m_lights) {
        m_light.view = glm::lookAt(
            glm::vec3(m_light.position),
            glm::vec3(0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f));
    }

    m_lightInfo.Update(m_device, &m_lights, sizeof(m_lights));
}
