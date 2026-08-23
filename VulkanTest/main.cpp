// main.cpp - Vulkan OBJ Renderer 
// Programmed by: Judith Caralt Rico

#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "vulkan.h"
#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "vulkan_setup.h"
#include "model.h"
#include "texture.h"

struct SceneObject {
    const char* objPath;
    const char* texturePath;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    // 0 = sin animacion, 1 = Mario salta, 2 = Goomba anda,
    // 4 = estrella gira, 5 = Lakitu flota,
    // 6 = particulas de brillo, 7 = esfera de cielo.
    int animation;

    std::vector<Vertex> vertices;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory uniformBufferMemory = VK_NULL_HANDLE;
    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

std::vector<VkCommandBuffer> commandBuffers;

VkSemaphore imageAvailableSemaphore;
VkSemaphore renderFinishedSemaphore;

VkDescriptorPool descriptorPool;
VkSampler textureSampler;
VkBuffer lightBuffer;
VkDeviceMemory lightBufferMemory;

std::vector<SceneObject> sceneObjects = {
    
    { "assets/sphere.obj", "assets/mariosky.bmp",
      glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(35.0f), 7 },

    { "assets/plane.obj", "assets/bricks.bmp",
      glm::vec3(0.0f, 0.0f, -0.9f), glm::vec3(0.0f), glm::vec3(8.0f, 8.0f, 1.0f), 0 },

    { "assets/mario.obj", "assets/mario.bmp",
      glm::vec3(0.0f, -0.9f, -0.25f), glm::vec3(0.0f, 0.0f, 180.0f), glm::vec3(0.55f), 1 },

    { "assets/goomba.obj", "assets/goomba.png",
      glm::vec3(-1.6f, 0.7f, -0.85f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.12f), 2 },

    { "assets/warp_pipe.obj", "assets/warp_pipe.png",
      glm::vec3(2.3f, -0.2f, -0.9f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.035f), 0 },

    { "assets/marioblock.obj", "assets/marioblock.png",
      glm::vec3(0.0f, -0.9f, 1.15f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.85f), 0 },

    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(0.0f, -0.9f, 1.55f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.16f), 4 },

    { "assets/lakitu.obj", "assets/lakitu.bmp",
      glm::vec3(2.3f, -0.2f, 0.55f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.075f), 5 },

    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(-0.35f, -0.9f, 2.05f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.050f), 6 },
    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(0.35f, -0.9f, 2.05f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.045f), 6 },
    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(0.0f, -0.55f, 2.20f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.040f), 6 },
    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(0.0f, -1.25f, 2.00f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.045f), 6 },
    { "assets/star.obj", "assets/rainbowStarMario.bmp",
      glm::vec3(0.0f, -0.9f, 2.35f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.038f), 6 }
};

glm::vec3 cameraPos = glm::vec3(0.0f, -5.2f, 1.4f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 1.0f, -0.12f));
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
float yaw = 90.0f;
float pitch = -7.0f;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void initWindow();
void processInput(GLFWwindow* window);
void createDescriptorSetLayout();
void createUniformBuffer(VkBuffer& buffer, VkDeviceMemory& memory);
void createLightBuffer();
void loadSceneResources();
void updateDescriptorPool();
void createCommandBuffers();
void createSemaphores();
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
void updateUniformBuffer(VkBuffer uboBuffer, VkDeviceMemory uboMemory, const glm::mat4& modelMat);
void updateLightBuffer();
void drawFrame();
void cleanup();

int main() {
    try {
        initWindow();
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        createCommandPool();
        createSwapChain();
        createDepthResources();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createFramebuffers();

        createTextureSampler(textureSampler);
        createLightBuffer();
        loadSceneResources();

        updateDescriptorPool();
        createCommandBuffers();
        createSemaphores();

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            processInput(window);
            drawFrame();
        }

        vkDeviceWaitIdle(device);
        cleanup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void DestroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, pAllocator);
    }
}

void mouseCallback(GLFWwindow*, double xpos, double ypos) {
    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    const float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.z = sin(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Final Scene", nullptr, nullptr);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void processInput(GLFWwindow* window) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 2.8f * deltaTime;
    glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, cameraFront.y, 0.0f));
    glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraSpeed * right;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos.z -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos.z += cameraSpeed;
}

void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding lightLayoutBinding{};
    lightLayoutBinding.binding = 2;
    lightLayoutBinding.descriptorCount = 1;
    lightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightLayoutBinding.pImmutableSamplers = nullptr;
    lightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
        uboLayoutBinding, samplerLayoutBinding, lightLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor set layout!");
}

void createUniformBuffer(VkBuffer& buffer, VkDeviceMemory& memory) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create uniform buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    bool found = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags &
                (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.memoryTypeIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
        throw std::runtime_error("failed to find suitable uniform buffer memory!");

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate uniform buffer memory!");

    vkBindBufferMemory(device, buffer, memory, 0);
}

void createLightBuffer() {
    VkDeviceSize bufferSize = sizeof(LightUniform);

    VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &lightBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create light buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, lightBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    bool found = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags &
                (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.memoryTypeIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
        throw std::runtime_error("failed to find suitable light buffer memory!");

    if (vkAllocateMemory(device, &allocInfo, nullptr, &lightBufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate light buffer memory!");

    vkBindBufferMemory(device, lightBuffer, lightBufferMemory, 0);
}

void loadSceneResources() {
    for (SceneObject& object : sceneObjects) {
        loadModel(object.objPath, object.vertices);
        createVertexBuffer(object.vertices, object.vertexBuffer, object.vertexBufferMemory);
        createUniformBuffer(object.uniformBuffer, object.uniformBufferMemory);
        createTextureImage(object.texturePath, object.textureImage, object.textureImageMemory);
        createTextureImageView(object.textureImage, object.textureImageView);
    }
}

glm::mat4 modelMatrixFor(const SceneObject& object, float time) {
    glm::vec3 pos = object.position;

    if (object.animation == 7) {
        pos = cameraPos;
    }

 
    if (object.animation == 1) {
        pos.z += std::abs(sin(time * 1.8f)) * 0.65f;
    }
    if (object.animation == 2) {
        pos.x += sin(time * 1.5f) * 0.75f;
    }
    if (object.animation == 4) {
        pos.z += std::abs(sin(time * 1.8f)) * 1.10f;
    }
    if (object.animation == 5) {
        pos.z += std::abs(sin(time * 1.2f)) * 0.80f;
    }
    if (object.animation == 6) {
        float seed = object.position.x * 4.0f + object.position.y * 2.0f;
        pos.x += sin(time * 2.4f + seed) * 0.12f;
        pos.y += cos(time * 2.2f + seed) * 0.12f;
        pos.z += std::abs(sin(time * 3.0f + seed)) * 0.22f;
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(object.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(object.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(object.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    if (object.animation == 4 || object.animation == 6) {
        model = glm::rotate(model, time * 1.8f, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    model = glm::scale(model, object.scale);
    return model;
}

void updateUniformBuffer(VkBuffer uboBuffer, VkDeviceMemory uboMemory, const glm::mat4& modelMat) {
    UniformBufferObject ubo{};
    ubo.model = modelMat;
    ubo.view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        swapChainExtent.width / static_cast<float>(swapChainExtent.height),
        0.1f,
        100.0f
    );
    ubo.proj[1][1] *= -1;
    ubo.color = glm::vec4(1.0f);

    void* data;
    vkMapMemory(device, uboMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uboMemory);
}

void updateLightBuffer() {
    LightUniform lightData{};

    // Una luz puntual sencilla para el modelo Phong.
    lightData.lightPos = glm::vec4(-3.5f, -2.0f, 4.0f, 1.0f);
    lightData.lightColor = glm::vec4(1.0f, 0.92f, 0.78f, 1.0f);
    lightData.cameraPos = glm::vec4(cameraPos, 1.0f);

    void* data;
    vkMapMemory(device, lightBufferMemory, 0, sizeof(lightData), 0, &data);
    memcpy(data, &lightData, sizeof(lightData));
    vkUnmapMemory(device, lightBufferMemory);
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearValues[2];
    clearValues[0].color = { {0.02f, 0.025f, 0.035f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkDeviceSize offsets[] = { 0 };
    for (const SceneObject& object : sceneObjects) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout, 0, 1, &object.descriptorSet, 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, offsets);
        vkCmdDraw(commandBuffer, static_cast<uint32_t>(object.vertices.size()), 1, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
}

void drawFrame() {
    const float time = static_cast<float>(glfwGetTime());
    for (SceneObject& object : sceneObjects) {
        updateUniformBuffer(object.uniformBuffer, object.uniformBufferMemory, modelMatrixFor(object, time));
    }
    updateLightBuffer();

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
        imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
    vkQueueWaitIdle(graphicsQueue);
}

void updateDescriptorPool() {
    const uint32_t objectCount = static_cast<uint32_t>(sceneObjects.size());

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = objectCount * 2;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = objectCount;

    VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = objectCount;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");

    std::vector<VkDescriptorSetLayout> layouts(objectCount, descriptorSetLayout);
    std::vector<VkDescriptorSet> descriptorSets(objectCount);

    VkDescriptorSetAllocateInfo allocInfoDS{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfoDS.descriptorPool = descriptorPool;
    allocInfoDS.descriptorSetCount = objectCount;
    allocInfoDS.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(device, &allocInfoDS, descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");

    VkDescriptorBufferInfo lightInfo{ lightBuffer, 0, sizeof(LightUniform) };

    for (uint32_t i = 0; i < objectCount; ++i) {
        sceneObjects[i].descriptorSet = descriptorSets[i];

        VkDescriptorBufferInfo uboInfo{ sceneObjects[i].uniformBuffer, 0, sizeof(UniformBufferObject) };

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = sceneObjects[i].textureImageView;
        imageInfo.sampler = textureSampler;

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
        descriptorWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr,
            sceneObjects[i].descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr, &uboInfo, nullptr };
        descriptorWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr,
            sceneObjects[i].descriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            &imageInfo, nullptr, nullptr };
        descriptorWrites[2] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr,
            sceneObjects[i].descriptorSet, 2, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            nullptr, &lightInfo, nullptr };

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void createCommandBuffers() {
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");

    for (size_t i = 0; i < commandBuffers.size(); i++)
        recordCommandBuffer(commandBuffers[i], static_cast<uint32_t>(i));
}

void createSemaphores() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
        throw std::runtime_error("failed to create semaphores!");
}

void cleanup() {
    vkDestroySampler(device, textureSampler, nullptr);

    for (SceneObject& object : sceneObjects) {
        vkDestroyImageView(device, object.textureImageView, nullptr);
        vkDestroyImage(device, object.textureImage, nullptr);
        vkFreeMemory(device, object.textureImageMemory, nullptr);
        vkDestroyBuffer(device, object.uniformBuffer, nullptr);
        vkFreeMemory(device, object.uniformBufferMemory, nullptr);
        vkDestroyBuffer(device, object.vertexBuffer, nullptr);
        vkFreeMemory(device, object.vertexBufferMemory, nullptr);
    }

    vkDestroyBuffer(device, lightBuffer, nullptr);
    vkFreeMemory(device, lightBufferMemory, nullptr);

    vkDestroyImageView(device, depthImageView, nullptr);
    vkDestroyImage(device, depthImage, nullptr);
    vkFreeMemory(device, depthImageMemory, nullptr);

    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

    vkDestroyCommandPool(device, commandPool, nullptr);

    for (auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);

    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

    for (auto imageView : swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}
