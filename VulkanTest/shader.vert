#version 450

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 color; // Optional tint color (not used for lighting calculations)
} ubo;

layout(location = 0) in vec3 a_vertex;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;

layout(location = 0) out vec2 v_uv;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec3 v_vertex;

void main() {
    v_vertex = (ubo.model * vec4(a_vertex, 1.0)).xyz;
    v_normal = mat3(transpose(inverse(ubo.model))) * a_normal;
    v_uv = a_uv;

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(a_vertex, 1.0);
}
