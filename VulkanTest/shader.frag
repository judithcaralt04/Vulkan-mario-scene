#version 450

layout(location = 0) in vec2 v_uv;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_vertex;

layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform Light {
    vec4 lightPos;
    vec4 lightColor;
    vec4 cameraPos;
} light;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(light.lightPos.xyz - v_vertex);
    vec3 V = normalize(light.cameraPos.xyz - v_vertex);
    vec3 R = reflect(-L, N);

    // Phong: ambiente + difuso + especular.
    float diffuseFactor = max(dot(N, L), 0.0);
    float specularFactor = pow(max(dot(V, R), 0.0), 32.0);
    vec3 ambient = vec3(0.18);
    vec3 diffuse = diffuseFactor * light.lightColor.rgb;
    vec3 specular = 0.35 * specularFactor * light.lightColor.rgb;
    vec3 phong = ambient + diffuse + specular;

    vec4 texColor = texture(texSampler, v_uv);
    fragColor = vec4(texColor.rgb * min(phong, vec3(1.25)), texColor.a);
}
