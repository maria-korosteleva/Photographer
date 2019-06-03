#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPosition;  // in world coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // info for fragment shader
    FragPosition = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;

    // avoid scaling issues
    // TODO move Normal matrix to the uniforms for a more efficient implementation
    Normal = mat3(transpose(inverse(model))) *  aNormal;

    // final coordinates
    gl_Position = projection * view * vec4(FragPosition, 1.0);
}