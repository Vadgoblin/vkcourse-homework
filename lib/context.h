#pragma once

#include "../HF1/LightningPass.h"
#include "../HF1/TextureManager.h"

#include <descriptors.h>
#include <string>
#include <vector>

#include <vulkan/vulkan_core.h>

class Context {
public:
    Context(const std::string& appName, bool useValidation)
        : m_appName(appName), m_useValidation(useValidation)
    {
    }

    // Disable copy and move constructors
    Context(const Context& otherCtx) = delete;
    Context(Context&& otherCtx)      = delete;

    VkInstance       CreateInstance(const std::vector<const char*>& layers, const std::vector<const char*>& extensions);
    VkPhysicalDevice SelectPhysicalDevice(const VkSurfaceKHR surface);
    VkDevice         CreateDevice(const std::vector<const char*>& extensions);
    VkCommandPool    CreateCommandPool();
    void             SetLightingPass(LightningPass* lightningPass){m_lightningPass = lightningPass;}

    void             Destroy();

    VkInstance       instance() const { return m_instance; }
    VkPhysicalDevice physicalDevice() const { return m_phyDevice; }
    VkDevice         device() const { return m_device; }
    uint32_t         queueFamilyIdx() const { return m_queueFamilyIdx; }
    VkQueue          queue() const { return m_queue; }
    VkSampleCountFlagBits sampleCountFlagBits() const {return m_SampleCountFlagBits;};
    VkCommandPool    commandPool() const { return m_commandPool; }
    DescriptorPool&  descriptorPool() { return m_descriptorPool; }
    LightningPass&   lightningPass() { return *m_lightningPass; }

protected:
    DescriptorPool   CreateDescriptorPool(const std::unordered_map<VkDescriptorType, uint32_t>& countPerType, uint32_t maxSets);
    bool FindQueueFamily(const VkPhysicalDevice phyDevice, const VkSurfaceKHR surface, uint32_t* outQueueFamilyIdx);
    void SetSampleCountFlagBits();

    const std::string m_appName;
    const bool        m_useValidation;

    VkInstance       m_instance       = VK_NULL_HANDLE;
    VkPhysicalDevice m_phyDevice      = VK_NULL_HANDLE;
    VkDevice         m_device         = VK_NULL_HANDLE;
    uint32_t         m_queueFamilyIdx = -1;
    VkQueue          m_queue          = VK_NULL_HANDLE;
    VkSampleCountFlagBits m_SampleCountFlagBits = static_cast<VkSampleCountFlagBits>(0);


    VkCommandPool    m_commandPool    = VK_NULL_HANDLE;
    DescriptorPool   m_descriptorPool = {};

    LightningPass*   m_lightningPass;
};
