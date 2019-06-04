#version 330 core
struct Material {
    float shininess;
    sampler2D specular;
    sampler2D diffuse;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cut_off;  // cosine value as a cut-off
    float outer_cut_off;

    vec3 ambient;   // control the intensity
    vec3 diffuse;   // main light color
    vec3 specular;  // usually just white

    // attenuation parameters
    float constant;
    float linear;
    float quadratic;
};

// output color!
out vec4 frag_color;

// From Vertex shader
in vec2 tex_coord;
in vec3 normal;
in vec3 frag_position;  // in world coordinates

// Uniform properties
uniform Material material;
uniform Light light;
uniform vec3 eye_pos;

void main()
{
    // ambient exist independently from the spotlight conditions
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coord));

    // light restrictions
    vec3 norm = normalize(normal);
    float attenuation = 1.0;
    vec3 light_dir = normalize(vec3(light.position) - frag_position);
    float theta = dot(light_dir, normalize(-light.direction));

    //if (theta > light.outer_cut_off)
    {
        float intensity = clamp((theta - light.outer_cut_off) / (light.cut_off - light.outer_cut_off), 0.0, 1.0);

        // diffuse
        float diffStrength = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = diffStrength * light.diffuse * vec3(texture(material.diffuse, tex_coord));

        // specular
        // TODO efficient calculation through approximation
        vec3 viewDir = normalize(eye_pos - frag_position);
        vec3 reflect_dir = reflect(-light_dir, norm);
        float specularStrength = pow(max(dot(norm, reflect_dir), 0.0), material.shininess);
        vec3 specular = specularStrength * light.specular * vec3(texture(material.specular, tex_coord));

        // all components together
        frag_color = vec4(attenuation *  intensity * (diffuse + specular) + ambient, 1.0);
    }
    /*else
    {
        frag_color = vec4(ambient, 1.0);
    }*/
    //float attenuation;
    //if (light.direction.w > 0.0)
    //{ // point light
    //    light_dir = normalize(vec3(light.direction) - frag_position);
    //    float dist  =   length(vec3(light.direction) - frag_position);
    //    attenuation =   1.0 / (light.constant + light.linear * dist + 
    //                    light.quadratic * dist * dist);
    //}     
    //else 
    //{ // directional light
    //    light_dir = normalize(vec3(light.direction));
    //    attenuation = 1.0;
    //}

}