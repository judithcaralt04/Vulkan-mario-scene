#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "vulkan_setup.h"

// Vertex structure 
struct Vertex {
    float pos[3];
    float normal[3];
    float texCoord[2];
};

void loadModel(const char* filename, std::vector<Vertex>& vertices);
void createVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory);

#endif

