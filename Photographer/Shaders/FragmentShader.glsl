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
uniform vec3 eyePos;

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
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * lightColor;

    // specular
    float specularWeight = 0.8;
    int shininess = 128;
    // TODO efficient calculation through approximation
    vec3 viewDir = normalize(eyePos - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = specularWeight * pow(max(dot(norm, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * lightColor;

    // all light components together
    vec3 result = (diffuse + ambient + specular) * objColor.xyz;
    FragColor = vec4(result, 1.0);
}