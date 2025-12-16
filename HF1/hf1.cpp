#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "texture.h"
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include "LightManager.h"

#include <vulkan/vulkan.h>

#include "glm_config.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "camera.h"
#include "context.h"
#include "debug.h"

#include "LightningPass.h"
#include "ObjectManager.h"
#include "imgui_integration.h"
#include "swapchain.h"
#include "wrappers.h"

#include "primitives/BasePrimitive.h"


void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));

    switch (key) {
    case GLFW_KEY_ESCAPE: {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    }
    case GLFW_KEY_W:
        camera->Forward();
        break;
    case GLFW_KEY_S:
        camera->Back();
        break;
    case GLFW_KEY_A:
        camera->Left();
        break;
    case GLFW_KEY_D:
        camera->Right();
        break;
    case GLFW_KEY_Q:
        camera->Down();
        break;
    case GLFW_KEY_E:
        camera->Up();
        break;
    }
}

void HandleJoystick(Camera* camera)
{
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
            const float deltaTime = 0.016f; // or calculate frame delta
            camera->ProcessControllerInput(state, deltaTime);
        }
    }
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    static bool  firstMouse = true;
    static float lastX      = 0.0f;
    static float lastY      = 0.0f;

    ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);

    Camera* camera = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (!ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX      = xpos;
            lastY      = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;

        lastX = xpos;
        lastY = ypos;

        camera->ProcessMouseMovement(xoffset, yoffset);
    } else {
        firstMouse = true;
    }
}

void RenderImGui(IMGUIIntegration imIntegration, const Camera& camera)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::GetIO().IniFilename = nullptr;
    imIntegration.NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    ImGui::SetNextWindowPos(ImVec2(15, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(358, 87), ImGuiCond_FirstUseEver);
    ImGui::Begin("Info");
    const glm::vec3& cameraPosition = camera.position();
    ImGui::Text("Camera position x: %.3f y: %.3f z: %.3f", cameraPosition.x, cameraPosition.y,
                cameraPosition.z);
    const glm::vec3& targetPosition = camera.lookAtPosition();
    ImGui::Text("Target position x: %.3f y: %.3f z: %.3f", targetPosition.x, targetPosition.y,
                targetPosition.z);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();


    ImGui::SetNextWindowPos(ImVec2(15, 115), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(224, 209), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls:");
    ImGui::Text("Movement control:");
    ImGui::Text("w - forward");
    ImGui::Text("d - backward");
    ImGui::Text("a - left");
    ImGui::Text("d - right");
    ImGui::Text("q - down");
    ImGui::Text("e - up");
    ImGui::Text(" ");
    ImGui::Text("Camera control:");
    ImGui::Text("mouse left click");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(15, 332), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(187, 158), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls (controller):");
    ImGui::Text("Movement control:");
    ImGui::Text("Left Stick");
    ImGui::Text("Left Trigger - down");
    ImGui::Text("Right Trigger - up");
    ImGui::Text(" ");
    ImGui::Text("Camera control:");
    ImGui::Text("Right Strick");
    ImGui::End();

    ImGui::Render();
}

int main(int /*argc*/, char** /*argv*/)
{
    if (glfwVulkanSupported()) {
        printf("Failed to look up minimal Vulkan loader/ICD\n!");
        return -1;
    }

    if (!glfwInit()) {
        printf("Failed to init GLFW!\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    uint32_t     count          = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);

    printf("Minimal set of requred extension by GLFW:\n");
    for (uint32_t idx = 0; idx < count; idx++) {
        printf("-> %s\n", glfwExtensions[idx]);
    }

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + count);

    Context    context("vkcourse hf1", true);
    VkInstance instance = context.CreateInstance({}, extensions);

    debug::setDebugUtilsObjectName(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
        vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")));

    uint32_t    windowWidth  = 1700;
    uint32_t    windowHeight = 900;
    GLFWwindow* window       = glfwCreateWindow(windowWidth, windowHeight, "hf1 - h257398", NULL, NULL);

    Camera camera({windowWidth, windowHeight}, 45.0f, 0.1f, 100.0f);

    IMGUIIntegration imIntegration;
    imIntegration.Init(window);

    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);


    // We have the window, the instance, create a surface from the window to draw onto.
    // Create a Vulkan Surface using GLFW.
    // By using GLFW the current windowing system's surface is created (xcb, win32, etc..)
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        // TODO: not the best, but will fail the application surely
        throw std::runtime_error("Failed to create window surface!");
    }

    VkPhysicalDevice phyDevice      = context.SelectPhysicalDevice(surface);
    VkDevice         device         = context.CreateDevice({});
    uint32_t         queueFamilyIdx = context.queueFamilyIdx();
    VkQueue          queue          = context.queue();

    Swapchain swapchain(instance, phyDevice, device, surface, {windowWidth, windowHeight});
    VkResult  swapchainCreated = swapchain.Create();
    assert(swapchainCreated == VK_SUCCESS);

    VkCommandPool cmdPool = context.CreateCommandPool();

    std::vector<VkCommandBuffer> cmdBuffers = AllocateCommandBuffers(device, cmdPool, swapchain.images().size());

    VkFence     imageFence       = CreateFence(device);
    VkSemaphore presentSemaphore = CreateSemaphore(device);

    imIntegration.CreateContext(context, swapchain);

    camera.CreateVK(context.device());

    Texture* depthTexture = Texture::Create2D(context.physicalDevice(), context.device(), VK_FORMAT_D32_SFLOAT,
                                             swapchain.surfaceExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, context.sampleCountFlagBits());

    Texture* msaaColorTexture;
    if (context.sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT) {
        msaaColorTexture = Texture::Create2D(context.physicalDevice(), context.device(), swapchain.format(),
                                             swapchain.surfaceExtent(),
                                             VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
                                             context.sampleCountFlagBits());
    }


    TextureManager textureManager(context);

    LightningPass lightningPass(context,textureManager, swapchain.format());

    ObjectManager objectManager(context, lightningPass);


    glfwShowWindow(window);

    const VkViewport viewport = {
        .x        = 0,
        .y        = 0,
        .width    = (float)swapchain.surfaceExtent().width,
        .height   = (float)swapchain.surfaceExtent().height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = swapchain.surfaceExtent(),
    };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        camera.Update();
        HandleJoystick(&camera);

        RenderImGui(imIntegration, camera);

        // Get new image to render to
        vkResetFences(device, 1, &imageFence);
        const Swapchain::Image& swapchainImage = swapchain.AquireNextImage(imageFence);
        vkWaitForFences(device, 1, &imageFence, VK_TRUE, UINT64_MAX);

        // Get command buffer based on swapchain image index
        VkCommandBuffer cmdBuffer = cmdBuffers[swapchainImage.idx];
        {
            // Begin command buffer record
            const VkCommandBufferBeginInfo beginInfo = {
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .pInheritanceInfo = nullptr,
            };
            vkBeginCommandBuffer(cmdBuffer, &beginInfo);

            swapchain.CmdTransitionToRender(cmdBuffer, swapchainImage, queueFamilyIdx);



            // ai generated fix for Validation Error that appeared after system update
// --------------------------------------------------------------------------------------
// [INSERT THIS BLOCK HERE]
// --------------------------------------------------------------------------------------

// 1. Transition Depth Texture
// We discard the old content (UNDEFINED) and prepare it for writing (DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
VkImageMemoryBarrier depthBarrier{};
depthBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
depthBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
depthBarrier.srcAccessMask = 0;
depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
// NOTE: Make sure to get the VkImage handle, not the view!
depthBarrier.image = depthTexture->image();
depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT; // Add | VK_IMAGE_ASPECT_STENCIL_BIT if using stencil
depthBarrier.subresourceRange.baseMipLevel = 0;
depthBarrier.subresourceRange.levelCount = 1;
depthBarrier.subresourceRange.baseArrayLayer = 0;
depthBarrier.subresourceRange.layerCount = 1;

// 2. Transition MSAA Color Texture (Only if MSAA is active)
// If you are using MSAA, the render target is 'msaaColorTexture', not the swapchain directly.
// It also needs to be transitioned from UNDEFINED to COLOR_ATTACHMENT_OPTIMAL.
if (context.sampleCountFlagBits() != VK_SAMPLE_COUNT_1_BIT) {
    VkImageMemoryBarrier msaaBarrier{};
    msaaBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    msaaBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    msaaBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    msaaBarrier.srcAccessMask = 0;
    msaaBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    msaaBarrier.image = msaaColorTexture->image(); // Get the VkImage handle
    msaaBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    msaaBarrier.subresourceRange.baseMipLevel = 0;
    msaaBarrier.subresourceRange.levelCount = 1;
    msaaBarrier.subresourceRange.baseArrayLayer = 0;
    msaaBarrier.subresourceRange.layerCount = 1;

    // Batch the barriers together for performance
    VkImageMemoryBarrier barriers[] = {depthBarrier, msaaBarrier};
    vkCmdPipelineBarrier(cmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        0, 0, nullptr, 0, nullptr, 2, barriers);
} else {
    // Only issue the depth barrier
    vkCmdPipelineBarrier(cmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        0, 0, nullptr, 0, nullptr, 1, &depthBarrier);
}

// --------------------------------------------------------------------------------------
// [END INSERT]
// --------------------------------------------------------------------------------------

            // Begin render commands
            const VkClearValue                 clearColor      = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

            // 1. Determine which view to use safely
            VkImageView targetView = (context.sampleCountFlagBits() == VK_SAMPLE_COUNT_1_BIT)
                                   ? swapchainImage.view
                                   : msaaColorTexture->view();

            // 2. Determine resolve settings safely
            VkImageView resolveView = (context.sampleCountFlagBits() == VK_SAMPLE_COUNT_1_BIT)
                                    ? VK_NULL_HANDLE
                                    : swapchainImage.view;

            VkResolveModeFlagBits resolveMode = (context.sampleCountFlagBits() == VK_SAMPLE_COUNT_1_BIT)
                                              ? VK_RESOLVE_MODE_NONE
                                              : VK_RESOLVE_MODE_AVERAGE_BIT;

            VkImageLayout resolveLayout = (context.sampleCountFlagBits() == VK_SAMPLE_COUNT_1_BIT)
                                        ? VK_IMAGE_LAYOUT_UNDEFINED
                                        : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            // 3. Now initialize the struct safely
            VkRenderingAttachmentInfoKHR colorAttachment = {
                .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .pNext              = nullptr,
                .imageView          = targetView,          // ✅ Safe now
                .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .resolveMode        = resolveMode,         // ✅ Safe now
                .resolveImageView   = resolveView,         // ✅ Safe now
                .resolveImageLayout = resolveLayout,       // ✅ Safe now
                .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue         = clearColor,
            };

            if (context.sampleCountFlagBits() == VK_SAMPLE_COUNT_1_BIT) {
                colorAttachment.imageView          = swapchainImage.view;
                colorAttachment.resolveMode        = VK_RESOLVE_MODE_NONE;
                colorAttachment.resolveImageView   = VK_NULL_HANDLE;
                colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }

            const VkClearDepthStencilValue     depthClear      = {1.0f, 0u};
            const VkRenderingAttachmentInfoKHR depthAttachment = {
                .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
                .pNext              = nullptr,
                .imageView          = depthTexture->view(),
                .imageLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .resolveMode        = VK_RESOLVE_MODE_NONE,
                .resolveImageView   = VK_NULL_HANDLE,
                .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue         = {.depthStencil = depthClear},
            };
            const VkRenderingInfoKHR renderInfo = {
                .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
                .pNext                = nullptr,
                .flags                = 0,
                .renderArea           = {.offset = {0, 0}, .extent = {(uint32_t)windowWidth, (uint32_t)windowHeight}},
                .layerCount           = 1,
                .viewMask             = 0,
                .colorAttachmentCount = 1,
                .pColorAttachments    = &colorAttachment,
                .pDepthAttachment     = &depthAttachment,
                .pStencilAttachment   = nullptr};
            vkCmdBeginRendering(cmdBuffer, &renderInfo);

            vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
            vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

            camera.PushConstants(cmdBuffer);

            // lightManager.BindDescriptorSets(cmdBuffer);
            objectManager.Draw(cmdBuffer);

            // Render things
            imIntegration.Draw(cmdBuffer);

            // End render commands
            vkCmdEndRendering(cmdBuffer);

            // Finish up recording
            swapchain.CmdTransitionToPresent(cmdBuffer, swapchainImage, queueFamilyIdx);
            vkEndCommandBuffer(cmdBuffer);
        }

        // Execute recorded commands
        const VkSubmitInfo submitInfo = {
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 0,
            .pWaitSemaphores      = nullptr,
            .pWaitDstStageMask    = nullptr,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &cmdBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &presentSemaphore,
        };
        vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

        // Present current image
        swapchain.QueuePresent(queue, presentSemaphore);

        vkDeviceWaitIdle(device);
    }

    if (context.sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT) msaaColorTexture->Destroy(context.device());
    depthTexture->Destroy(context.device());
    imIntegration.Destroy(context);

    vkDestroyFence(device, imageFence, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);

    vkDestroyCommandPool(device, cmdPool, nullptr);

    camera.Destroy(device);
    lightningPass.Destroy();
    textureManager.Destroy();
    objectManager.Destroy(device);
    swapchain.Destroy();
    context.Destroy();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
