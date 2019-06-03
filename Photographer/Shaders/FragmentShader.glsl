#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    sampler2D texture1;
    sampler2D texture2;
    float mix_value;
};

struct Light {
    vec3 position;

    vec3 ambient;   // control the intensity
    vec3 diffuse;   // main light color
    vec3 specular;  // usually just white
};


// output color!
out vec4 FragColor;

// From Vertex shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPosition;  // in world coordinates

// Uniform properties
uniform Material material;
uniform Light light;
uniform vec3 eyePos;

void main()
{
    // base
    vec4 objColor = mix(texture(material.texture1, TexCoord), texture(material.texture2, TexCoord), material.mix_value);

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPosition);
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * light.diffuse * material.diffuse;

    // specular
    // TODO efficient calculation through approximation
    vec3 viewDir = normalize(eyePos - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = pow(max(dot(norm, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * light.specular * material.specular;

    // all light components together
    vec3 result = (diffuse + ambient + specular) * objColor.xyz;
    FragColor = vec4(result, 1.0);
}