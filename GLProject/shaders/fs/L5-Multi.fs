#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseTex;

uniform vec3 sunPos;     // Sun world position
uniform vec3 viewPos;

void main()
{
    vec3 albedo = texture(diffuseTex, TexCoords).rgb;

    vec3 N = normalize(Normal);
    vec3 L = normalize(sunPos - FragPos);

    // Sun light only
    float NdotL = max(dot(N, L), 0.0);

    // No ambient light (space is dark)
    vec3 color = albedo * NdotL;

    FragColor = vec4(color, 1.0);
}
