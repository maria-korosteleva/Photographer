#version 330 core
struct Material {
    float shininess;
    sampler2D specular;
    sampler2D diffuse;
};

struct Light {
    vec4 direction; // direction from the light source (point or directional)

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
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir;
    if (light.direction.w > 0.0)
    { // point light
        lightDir = normalize(vec3(light.direction) - FragPosition);
    }     
    else 
    { // directional light
        lightDir = normalize(-vec3(light.direction));
    }

    float diffStrength = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffStrength * light.diffuse * vec3(texture(material.diffuse, TexCoord));

    // specular
    // TODO efficient calculation through approximation
    vec3 viewDir = normalize(eyePos - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularStrength = pow(max(dot(norm, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * light.specular * vec3(texture(material.specular, TexCoord));

    // all components together
    FragColor = vec4(diffuse + ambient + specular, 1.0);
}