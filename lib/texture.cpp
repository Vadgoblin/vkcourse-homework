#include "texture.h"

#include <vulkan/vulkan_core.h>

#include "../HF1/debug.h"
#include "buffer.h"
#include "stb_image.h"

#include <cmath>

VkImageView Create2DImageView(
    const VkDevice  device,
    const VkFormat  format,
    const VkImage   image,
    const uint32_t  mipmap_level_count) {

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    if (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    VkImageViewCreateInfo createInfo = {
        .sType          = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext          = nullptr,
        .flags          = 0,
        .image          = image,   // will be updated below
        .viewType       = VK_IMAGE_VIEW_TYPE_2D,
        .format         = format,
        .components     = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask     = aspectMask,
            .baseMipLevel   = 0,
            .levelCount     = mipmap_level_count,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        }
    };

    VkImageView view = VK_NULL_HANDLE;
    VkResult result = vkCreateImageView(device, &createInfo, nullptr, &view);
    if (result != VK_SUCCESS) {
        // TODO: error report?
        return VK_NULL_HANDLE;
    }

    return view;
}

static uint32_t FindMemoryTypeIndex(const VkPhysicalDevice phyDevice, const VkMemoryRequirements& requirements, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(phyDevice, &memoryProperties);

    for (uint32_t idx = 0; idx < memoryProperties.memoryTypeCount; idx++) {
        if (requirements.memoryTypeBits & (1 << idx)) {
            const VkMemoryType& memoryType = memoryProperties.memoryTypes[idx];
            // TODO: add size check?

            if (memoryType.propertyFlags & flags) {
                return idx;
            }
        }
    }

    return (uint32_t)-1;
}

// ai generated
void GenerateMipmaps(VkCommandBuffer cmd, VkImage image, uint32_t texWidth, uint32_t texHeight, uint32_t mipLevels) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        // 1. Transition Level i-1 to TRANSFER_SRC_OPTIMAL (so we can read from it)
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr, 0, nullptr, 1, &barrier);

        // 2. Blit (Copy and resize) from Level i-1 to Level i
        VkImageBlit blit{};
        blit.srcOffsets[0] = {0, 0, 0};
        blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;

        blit.dstOffsets[0] = {0, 0, 0};
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(cmd,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        // 3. Transition Level i-1 to SHADER_READ_ONLY (We are done with it)
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr, 0, nullptr, 1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    // 4. Transition the very last level to SHADER_READ_ONLY
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr, 0, nullptr, 1, &barrier);
}

bool Texture::InitFromBuffer(
    const VkPhysicalDevice  phyDevice,
    const VkDevice          device,
    const VkQueue           queue,
    const VkCommandPool     cmdPool,
    VkImageUsageFlags       usage,
    const VkBuffer          buffer) {

    CreateImage(phyDevice, device, usage);

    UploadFromBuffer(device, queue, cmdPool, buffer);

    m_view = Create2DImageView(device, m_format, m_image, m_mipLevels);
    Create2DSampler(device, true);

    return true;
}

Texture* Texture::LoadFromFile(const VkPhysicalDevice phyDevice,
                               const VkDevice         device,
                               const VkQueue          queue,
                               const VkCommandPool    cmdPool,
                               const std::string&     path,
                               const VkFormat         format,
                               VkImageUsageFlags      usage)
{

    // 1) Load the image file contents
    int32_t width    = 0;
    int32_t height   = 0;
    int32_t channels = 0;

    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        return nullptr;
    }

    printf("Loaded image: %s (%dx%d)\n", path.c_str(), width, height);

    // 2) Upload image data to a staging buffer
    const uint32_t rawSize   = width * height * 4;
    BufferInfo     rawBuffer = BufferInfo::Create(phyDevice, device, rawSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    rawBuffer.Update(device, data, rawSize);

    stbi_image_free(data);

    Texture* texture = new Texture(format, width, height);
    texture->InitFromBuffer(phyDevice, device, queue, cmdPool,
                            usage | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                            rawBuffer.buffer);

    rawBuffer.Destroy(device);

    return texture;
}

Texture* Texture::Create2D(const VkPhysicalDevice phyDevice,
                           const VkDevice         device,
                           const VkFormat         format,
                           VkExtent2D             extent,
                           VkImageUsageFlags      usage,
                           VkSampleCountFlagBits  msaaSamples) {

    Texture *texture = new Texture(format, extent.width, extent.height);

    texture->CreateImage(phyDevice, device, usage, msaaSamples);

    static VkImageUsageFlags requiresView = 0
        | VK_IMAGE_USAGE_SAMPLED_BIT
        | VK_IMAGE_USAGE_STORAGE_BIT
        | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
        | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
    ;

    if ((usage & requiresView) != 0) {
        texture->m_view = Create2DImageView(device, texture->m_format, texture->m_image);
        if ((usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) {
            texture->Create2DSampler(device, false);
        }
    }

    return texture;
}


VkResult Texture::CreateImage(
    const VkPhysicalDevice  phyDevice,
    const VkDevice          device,
    VkImageUsageFlags       usage,
    VkSampleCountFlagBits   msaaSamples) {

    bool texture = true;
    if (msaaSamples != VK_SAMPLE_COUNT_1_BIT) {
        texture = false;
    }

    m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;

    VkImageCreateInfo createInfo = {
        .sType                  = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .imageType              = VK_IMAGE_TYPE_2D,
        .format                 = m_format,
        .extent                 = { m_width, m_height, 1 },
        .mipLevels              = texture?m_mipLevels:1,
        .arrayLayers            = 1,
        .samples                = texture?VK_SAMPLE_COUNT_1_BIT: msaaSamples,
        .tiling                 = VK_IMAGE_TILING_OPTIMAL,
        .usage                  = usage,
        .sharingMode            = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount  = 0,
        .pQueueFamilyIndices    = nullptr,
        .initialLayout          = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkResult createResult = vkCreateImage(device, &createInfo, nullptr, &m_image);
    (void)createResult;

    VkMemoryRequirements requirements = {};
    vkGetImageMemoryRequirements(device, m_image, &requirements);

    const uint32_t memoryTypeIdx = FindMemoryTypeIndex(phyDevice, requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    // TODO: check for error

    VkMemoryAllocateInfo allocInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = 0,
        .allocationSize  = requirements.size,
        .memoryTypeIndex = memoryTypeIdx,
    };

    vkAllocateMemory(device, &allocInfo, nullptr, &m_memory);
    debug::SetDebugObjectName(device,VK_OBJECT_TYPE_DEVICE_MEMORY,(uint64_t)m_memory,"Texture::CreateImage m_memory");
    vkBindImageMemory(device, m_image, m_memory, 0);

    return VK_SUCCESS;
}

void Texture::Destroy(const VkDevice device) {
    vkDestroySampler(device, m_sampler, nullptr);
    vkDestroyImageView(device, m_view, nullptr);
    vkDestroyImage(device, m_image, nullptr);
    vkFreeMemory(device, m_memory, nullptr);
}

bool Texture::Create2DSampler(const VkDevice device, bool texture = false) {
    VkSamplerCreateInfo createInfo = {
        .sType              = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .magFilter          = VK_FILTER_LINEAR,
        .minFilter          = VK_FILTER_LINEAR,
        .mipmapMode         = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW       = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias         = 0,
        .anisotropyEnable   = VK_TRUE,
        .maxAnisotropy      = 16.0f,
        .compareEnable      = VK_FALSE,
        .compareOp          = VK_COMPARE_OP_NEVER,
        .minLod             = 0.0f,
        .maxLod             = texture?static_cast<float>(m_mipLevels):0.0f,
        .borderColor        = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    VkResult result = vkCreateSampler(device, &createInfo, nullptr, &m_sampler);

    return result == VK_SUCCESS;
}


bool Texture::UploadFromBuffer(
    const VkDevice      device,
    const VkQueue       queue,
    const VkCommandPool cmdPool,
    const VkBuffer&     rawBuffer) {
    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;

    VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = cmdPool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1u,
    };

    // TODO: check result
    vkAllocateCommandBuffers(device, &allocInfo, &cmdBuffer);

    VkCommandBufferBeginInfo beginInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext              = nullptr,
        .flags              = 0,
        .pInheritanceInfo   = nullptr,
    };

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);

    VkImageMemoryBarrier startBarrier = {
        .sType                  = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext                  = nullptr,
        .srcAccessMask          = 0,
        .dstAccessMask          = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout              = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout              = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex    = 0,
        .dstQueueFamilyIndex    = 0,
        .image                  = m_image,
        .subresourceRange       = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
    };

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &startBarrier);

    VkBufferImageCopy range = {
        .bufferOffset       = 0,
        .bufferRowLength    = 0,
        .bufferImageHeight  = 0,
        .imageSubresource   = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
        .imageOffset        = { 0, 0, 0 },
        .imageExtent        = { m_width, m_height, 1 },
    };


    // 1. Transition UNDEFINED -> TRANSFER_DST
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // Target layout for the Copy
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(cmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    // 2. NOW you can copy
    vkCmdCopyBufferToImage(cmdBuffer, rawBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &range);

    // 3. Then generate mipmaps (transitions to READ_ONLY)
    GenerateMipmaps(cmdBuffer, m_image, m_width, m_height, m_mipLevels);


    // vkCmdCopyBufferToImage(cmdBuffer, rawBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &range);

    // GenerateMipmaps(cmdBuffer, m_image, m_width, m_height, m_mipLevels);

    // VkImageMemoryBarrier endBarrier = startBarrier;
    // endBarrier.srcAccessMask    = startBarrier.dstAccessMask;
    // endBarrier.oldLayout        = startBarrier.newLayout;
    // endBarrier.newLayout        = VK_IMAGE_LAYOUT_GENERAL;
    //
    // vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &endBarrier);

    vkEndCommandBuffer(cmdBuffer);

    // Submit
    VkSubmitInfo submitInfo = {
        .sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                  = nullptr,
        .waitSemaphoreCount     = 0,
        .pWaitSemaphores        = nullptr,
        .pWaitDstStageMask      = nullptr,
        .commandBufferCount     = 1,
        .pCommandBuffers        = &cmdBuffer,
        .signalSemaphoreCount   = 0,
        .pSignalSemaphores      = nullptr,
    };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkDeviceWaitIdle(device);

    return true;
}

