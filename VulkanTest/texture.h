#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdexcept>


#include "vulkan.h"
#include "vulkan_setup.h"





void createTextureImage(const char* filename, VkImage& textureImage, VkDeviceMemory& textureImageMemory);
void createTextureImageView(VkImage textureImage, VkImageView& textureImageView);
void createTextureSampler(VkSampler &textureSampler);

#endif