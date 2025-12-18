#include <cstdio>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <vulkan/vulkan_core.h>

#include "camera.h"
#include "context.h"
#include "debug.h"
#include "glm_config.h"
#include "imgui_integration.h"
#include "managers/LightManager.h"
#include "managers/ObjectManager.h"
#include "managers/TextureManager.h"
#include "primitives/BasePrimitive.h"
#include "render_passes/LightningPass.h"
#include "render_passes/PostProcessPass.h"
#include "render_passes/ShadowPass.h"
#include "swapchain.h"
#include "wrappers.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vulkan/vulkan.h>

bool             showInfo        = true;
constexpr double press_timeout = 0.5;
double last_press_time = 0;

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

    case GLFW_KEY_H:
        double press_time = glfwGetTime();
        if (press_time - last_press_time <= press_timeout)return;

        last_press_time = press_time;
        showInfo = !showInfo;
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
    ImGuiIO& io                = ImGui::GetIO();
    ImGui::GetIO().IniFilename = nullptr;
    imIntegration.NewFrame();
    ImGui::NewFrame();
    if (showInfo) {
        ImGui::SetNextWindowPos(ImVec2(15, 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(358, 97), ImGuiCond_FirstUseEver);
        ImGui::Begin("Info");
        const glm::vec3& cameraPosition = camera.position();
        ImGui::Text("Camera position x: %.3f y: %.3f z: %.3f", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        const glm::vec3& targetPosition = camera.lookAtPosition();
        ImGui::Text("Target position x: %.3f y: %.3f z: %.3f", targetPosition.x, targetPosition.y, targetPosition.z);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Press the key h to hide/show infos");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(15, 125), ImGuiCond_FirstUseEver);
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

        ImGui::SetNextWindowPos(ImVec2(15, 342), ImGuiCond_FirstUseEver);
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
    }

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

    VkFormat              depthFormat = VK_FORMAT_D32_SFLOAT;
    VkSampleCountFlagBits msaaLevel   = context.GetMaxSampleCountFlagBit();
    // VkSampleCountFlagBits msaaLevel = VK_SAMPLE_COUNT_1_BIT;

    TextureManager textureManager(context);
    LightManager   lightManager(context);

    uint32_t   shadowResolution = 2 * 1024;
    ShadowPass shadowPass(context, lightManager, depthFormat, {shadowResolution, shadowResolution});

    LightningPass lightningPass(context, textureManager, lightManager, shadowPass, swapchain.format(), msaaLevel,
                                depthFormat, swapchain.surfaceExtent());

    ObjectManager objectManager(context, lightningPass, shadowPass);

    PostProcessPass postProcess(swapchain.format(), swapchain.surfaceExtent());
    postProcess.Create(context);

    postProcess.BindInputImage(context.device(), lightningPass.colorOutput());

    // glfwShowWindow(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        camera.Update();
        HandleJoystick(&camera);

        objectManager.Tick();
        lightManager.Tick(0.6f);
        // lightManager.Tick(0.0f);

        RenderImGui(imIntegration, camera);

        // Get new image to render to
        vkResetFences(device, 1, &imageFence);

        const Swapchain::Image& swapchainImage = swapchain.AquireNextImage(imageFence);

        vkWaitForFences(device, 1, &imageFence, VK_TRUE, UINT64_MAX);

        // Get command buffer based on swapchain image index
        VkCommandBuffer cmdBuffer = cmdBuffers[swapchainImage.idx];

        // Begin command buffer record
        const VkCommandBufferBeginInfo beginInfo = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .pInheritanceInfo = nullptr,
        };
        vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        swapchain.CmdTransitionToRender(cmdBuffer, swapchainImage, queueFamilyIdx);

        shadowPass.DoPass(cmdBuffer, [&](VkCommandBuffer cmd) { objectManager.Draw(cmd, false); });

        lightningPass.DoPass(cmdBuffer, [&](VkCommandBuffer cmd) {
            lightManager.BindDescriptorSets(cmd, lightningPass.pipelineLayout());
            shadowPass.BindDescriptorSets(cmd, lightningPass.pipelineLayout());

            camera.PushConstants(cmd);
            objectManager.Draw(cmd, true);
        });

        postProcess.DoPass(cmdBuffer, swapchainImage, [&](VkCommandBuffer cmd) { imIntegration.Draw(cmd); });

        vkEndCommandBuffer(cmdBuffer);

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

    imIntegration.Destroy(context);

    vkDestroyFence(device, imageFence, nullptr);
    vkDestroySemaphore(device, presentSemaphore, nullptr);

    vkDestroyCommandPool(device, cmdPool, nullptr);

    camera.Destroy(device);
    postProcess.Destroy(context);
    lightningPass.Destroy();
    shadowPass.Destroy(device);
    lightManager.Destroy();
    textureManager.Destroy();
    objectManager.Destroy(device);
    swapchain.Destroy();
    context.Destroy();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
