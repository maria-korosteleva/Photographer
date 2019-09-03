#pragma once

#ifndef SHADER_CODE_GLSL_TO_STRING
#define SHADER_CODE_GLSL_TO_STRING(version, shader)  "#version " #version " core \n" #shader  
#endif

static const char *full_fragment_shader_source = SHADER_CODE_GLSL_TO_STRING(330,

    struct Material {
        float shininess;
        vec3 specular;
        vec3 diffuse;
    };

    struct DirectionalLight
    {
        vec3 direction;

        vec3 ambient;   // control the intensity
        vec3 diffuse;   // main light color
        vec3 specular;  // usually just white
    };

    struct PointLight
    {
        vec3 position;

        vec3 ambient;   // control the intensity
        vec3 diffuse;   // main light color
        vec3 specular;  // usually just white

        float attenuation_constant;
        float attenuation_linear;
        float attenuation_quadratic;
    };

    struct SpotLight
    {
        vec3 position;
        vec3 direction;
        float cut_off;  // cosine value as a cut-off
        float outer_cut_off;

        vec3 ambient;   // control the intensity
        vec3 diffuse;   // main light color
        vec3 specular;  // usually just white
    };

    // output color!
    out vec4 frag_color;

    // From Vertex shader
    in vec3 vs_normal;
	in vec2 vs_uv;
    in vec3 vs_frag_position;  // in world coordinates

    // Uniform properties
    uniform Material material;
    uniform vec3 eye_pos;
	uniform sampler2D Tex1;

    uniform DirectionalLight directional_light;
    const int NR_POINT_LIGHTS = 2;
    uniform PointLight point_lights[NR_POINT_LIGHTS];

    // calculators
    vec3 CalcAmbient    (vec3 light_ambient);
    vec3 CalcDiffuse    (vec3 light_diffuse, vec3 light_dir, vec3 normal);
    vec3 CalcSpecular   (vec3 light_specular, vec3 light_dir, vec3 normal, vec3 view_dir);
    vec3 CalcDirectionalLight   (DirectionalLight light, vec3 normal, vec3 view_dir);
    vec3 CalcPointLight         (PointLight light, vec3 normal, vec3 view_dir);
    vec3 CalcSpotLight          (SpotLight light, vec3 normal, vec3 view_dir);

    // ------------------ main ---------------------
    void main()
    {
        vec3 norm = normalize(vs_normal);
        vec3 view_dir = normalize(eye_pos - vs_frag_position);
    
        vec3 out_color = vec3(0.0);

        out_color += CalcDirectionalLight(directional_light, norm, view_dir);

        for (int i = 0; i < NR_POINT_LIGHTS; ++i)
        {
            out_color += CalcPointLight(point_lights[i], norm, view_dir);
        }

		//frag_color = vec4(out_color, 1.0);
		vec2 refine_vs_uv = vec2(vs_uv.x, 1.0 - vs_uv.y);
        frag_color = texture(Tex1, refine_vs_uv) * vec4(out_color, 1.0);
    }

    // -------------- implemetations ---------------
    vec3 CalcAmbient(vec3 light_ambient)
    {
        return light_ambient * material.diffuse;
    }

    vec3 CalcDiffuse(vec3 light_diffuse, vec3 light_dir, vec3 normal)
    {
        float diff_strength = max(dot(normal, light_dir), 0.0);
        return diff_strength * light_diffuse * material.diffuse;
    }

    vec3 CalcSpecular(vec3 light_specular, vec3 light_dir, vec3 normal, vec3 view_dir)
    {
        // TODO efficient calculation through approximation
        vec3 reflect_dir = reflect(-light_dir, normal);
        float specular_strength = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        return specular_strength * light_specular * material.specular;
    }

    vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir)
    {
        vec3 light_dir = normalize(-light.direction);

        // components
        vec3 ambient = CalcAmbient(light.ambient);
        vec3 diffuse = CalcDiffuse(light.diffuse, light_dir, normal);
        vec3 specular = CalcSpecular(light.specular, light_dir, normal, view_dir);

        return (diffuse + specular + ambient);
    }

    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 view_dir)
    {
        vec3 light_dir = normalize(light.position - vs_frag_position);
        float dist = length(light.position - vs_frag_position);

        // components
        vec3 ambient = CalcAmbient(light.ambient);
        vec3 diffuse = CalcDiffuse(light.diffuse, light_dir, normal);
        vec3 specular = CalcSpecular(light.specular, light_dir, normal, view_dir);

        // fading
        float attenuation = 1.0 / (light.attenuation_constant 
                                    + light.attenuation_linear * dist
                                    + light.attenuation_quadratic * dist * dist);

        return attenuation * (diffuse + specular + ambient);
    }

    vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 view_dir)
    {
        vec3 light_dir = normalize(light.position - vs_frag_position);

        // components
        vec3 ambient = CalcAmbient(light.ambient);
        vec3 diffuse = CalcDiffuse(light.diffuse, light_dir, normal);
        vec3 specular = CalcSpecular(light.specular, light_dir, normal, view_dir);

        float theta = dot(light_dir, normalize(-light.direction));
        float intensity = clamp((theta - light.outer_cut_off) / (light.cut_off - light.outer_cut_off), 0.0, 1.0);

        return intensity * (diffuse + specular + ambient);
    }

);