#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D diffuseTex;

uniform vec3 sunPos;
uniform vec3 moonPos;  
uniform vec3 viewPos;

void main()
{
    vec3 albedo = texture(diffuseTex, TexCoords).rgb;

    vec3 N = normalize(Normal);
     
    vec3 L_sun = normalize(sunPos - FragPos);
    float NdotL_sun = max(dot(N, L_sun), 0.0);
    vec3 sunColor = albedo * NdotL_sun;
     
    vec3 L_moon = normalize(moonPos - FragPos);
    float NdotL_moon = max(dot(N, L_moon), 0.0);
    vec3 moonColor = albedo * 0.2 * NdotL_moon;  

    vec3 color = sunColor + moonColor;

    FragColor = vec4(color, 1.0);
}
