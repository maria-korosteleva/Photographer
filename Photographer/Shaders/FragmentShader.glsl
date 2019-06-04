#version 330 core
struct Material {
    float shininess;
    sampler2D specular;
    sampler2D diffuse;
};

struct Light {
    vec4 direction; // direction to the light source (point or directional)

    vec3 ambient;   // control the intensity
    vec3 diffuse;   // main light color
    vec3 specular;  // usually just white

    // attenuation parameters
    float constant;
    float linear;
    float quadratic;
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
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // light directions
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    float attenuation;
    if (light.direction.w > 0.0)
    { // point light
        lightDir = normalize(vec3(light.direction) - FragPosition);
        float dist  =   length(vec3(light.direction) - FragPosition);
        attenuation =   1.0 / (light.constant + light.linear * dist + 
                        light.quadratic * dist * dist);
    }     
    else 
    { // directional light
        lightDir = normalize(vec3(light.direction));
        attenuation = 1.0;
    }

    // diffuse
    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * light.diffuse * vec3(texture(material.diffuse, TexCoord));

    // specular
    // TODO efficient calculation through approximation
    vec3 viewDir = normalize(eyePos - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = pow(max(dot(norm, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * light.specular * vec3(texture(material.specular, TexCoord));

    // all components together
    FragColor = vec4(attenuation * (diffuse + ambient + specular), 1.0);
}