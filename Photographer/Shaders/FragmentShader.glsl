#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;  // in world coordinates

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mix_value;

uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // base
    vec4 objColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mix_value);

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPosition);
    float diff_strength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff_strength * lightColor;

    // all light components together
    vec3 result = (diffuse + ambient) * objColor.xyz;
    //vec3 result = diffuse;
    FragColor = vec4(result, 1.0);
}