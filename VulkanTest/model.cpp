#include "model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

void loadModel(const char* filename, std::vector<Vertex>& vertices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Carga OBJ file.
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename))
        throw std::runtime_error("failed to load OBJ file: " + std::string(filename));

    bool computeNormals = attrib.normals.empty();

 
    vertices.clear();

  
    for (const auto& shape : shapes) {

        for (size_t i = 0; i + 2 < shape.mesh.indices.size(); i += 3) {
     
            const tinyobj::index_t& idx0 = shape.mesh.indices[i];
            const tinyobj::index_t& idx1 = shape.mesh.indices[i + 1];
            const tinyobj::index_t& idx2 = shape.mesh.indices[i + 2];

            glm::vec3 v0(
                attrib.vertices[3 * idx0.vertex_index + 0],
                attrib.vertices[3 * idx0.vertex_index + 1],
                attrib.vertices[3 * idx0.vertex_index + 2]
            );
            glm::vec3 v1(
                attrib.vertices[3 * idx1.vertex_index + 0],
                attrib.vertices[3 * idx1.vertex_index + 1],
                attrib.vertices[3 * idx1.vertex_index + 2]
            );
            glm::vec3 v2(
                attrib.vertices[3 * idx2.vertex_index + 0],
                attrib.vertices[3 * idx2.vertex_index + 1],
                attrib.vertices[3 * idx2.vertex_index + 2]
            );

            glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            for (int j = 0; j < 3; j++) {
                const tinyobj::index_t& idx = shape.mesh.indices[i + j];
                Vertex vertex{};
                vertex.pos[0] = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.pos[1] = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.pos[2] = attrib.vertices[3 * idx.vertex_index + 2];


                if (!computeNormals && idx.normal_index >= 0) {
                    glm::vec3 n(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                    n = glm::normalize(n);

                    vertex.normal[0] = n.x;
                    vertex.normal[1] = n.y;
                    vertex.normal[2] = n.z;
                }
                else {
                    vertex.normal[0] = faceNormal.x;
                    vertex.normal[1] = faceNormal.y;
                    vertex.normal[2] = faceNormal.z;
                }

                if (idx.texcoord_index >= 0) {
                    vertex.texCoord[0] = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.texCoord[1] = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else {
                    vertex.texCoord[0] = 0.0f;
                    vertex.texCoord[1] = 0.0f;
                }

                vertices.push_back(vertex);
            }
        }
    }

    std::cout << "Loaded " << filename << " with " << vertices.size() << " vertices." << std::endl;
}



// ----------------------------------------------------------------------------
// Create vertex buffer.
void createVertexBuffer(std::vector<Vertex> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory) {
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create vertex buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    bool found = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) ==
            (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            allocInfo.memoryTypeIndex = i;
            found = true;
            break;
        }
    }
    if (!found)
        throw std::runtime_error("failed to find suitable memory type!");

    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, vertexBufferMemory);
}
