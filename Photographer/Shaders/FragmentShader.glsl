#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;  // in world coordinates

out vec4 FragColor;

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

    // all light components together
    vec3 result = ambient * objColor.xyz;
    FragColor = vec4(result, 1.0);
}