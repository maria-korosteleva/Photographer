#version 330 core

in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mix_value;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mix_value);
}