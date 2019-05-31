#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mix_value;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mix_value);
    FragColor = vec4(objectColor * lightColor, 1.0);
}