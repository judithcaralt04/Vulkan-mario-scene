#ifndef VULKAN_SETUP_H
#define VULKAN_SETUP_H

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <array>

#include "vulkan.h"
#include "glfw3.h"

// Include GLM for matrix math.
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

#define VERTEX_SHADER "vert.spv"
#define FRAGMENT_SHADER "frag.spv"


// Enable Validation Layers
const bool enableValidationLayers = true;

// Window dimensions
const uint32_t WIDTH = 600;
const uint32_t HEIGHT = 600;


// Uniform Buffer Object for matrices and an optional color.
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 color;
};

// Datos de las luces para el shader Phong.
struct LightUniform {
    glm::vec4 lightPos;
    glm::vec4 lightColor;
    glm::vec4 cameraPos;
};


// Enable validation layers in debug builds
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

extern GLFWwindow* window;

// VULKAN GLOBALS
extern VkInstance instance;
extern VkSurfaceKHR surface;
extern VkPhysicalDevice physicalDevice;
extern VkDevice device;
extern VkQueue graphicsQueue;
extern VkCommandPool commandPool;
extern VkSwapchainKHR swapChain;
extern std::vector<VkImage> swapChainImages;
extern VkFormat swapChainImageFormat;
extern VkExtent2D swapChainExtent;
extern std::vector<VkImageView> swapChainImageViews;
extern VkRenderPass renderPass;
extern VkPipelineLayout pipelineLayout;
extern VkPipeline graphicsPipeline;
extern std::vector<VkFramebuffer> swapChainFramebuffers;

extern VkImage        depthImage;
extern VkDeviceMemory depthImageMemory;
extern VkImageView    depthImageView;
extern VkFormat       depthFormat;

extern VkDescriptorSetLayout descriptorSetLayout;

// Forward‐declare the debug messenger handle
extern VkDebugUtilsMessengerEXT debugMessenger;

//////////////////////////////////////////////////////////////

void createInstance();
void createSurface();
void pickPhysicalDevice();
void createLogicalDevice();
void createCommandPool();
void createSwapChain();
void createDepthResources();
void createImageViews();
void createRenderPass();
void createGraphicsPipeline();
void createFramebuffers();



uint32_t findQueueFamily();
VkCommandBuffer beginSingleTimeCommands();
void endSingleTimeCommands(VkCommandBuffer commandBuffer);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

#endif // VULKAN_SETUP_H
