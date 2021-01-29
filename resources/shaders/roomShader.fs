#version 330 core
   out vec4 FragColor;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

   in vec3 aColor;
   in vec2 TexCoords;
   in vec3 Normal;
   in vec3 FragPos;
   // texture samplers
   uniform sampler2D texture1;
   uniform sampler2D floor_texture;
   uniform vec3 viewPos;
   uniform SpotLight spotLight;
   uniform DirLight dirLight;

    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
    {
        vec3 lightDir = normalize(-light.direction);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
        vec3 diffuse = light.diffuse * diff * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
        vec3 specular = light.specular * spec * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
        return (ambient + diffuse + specular);
    }

   vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
   {
       vec3 lightDir = normalize(light.position - fragPos);
       // diffuse shading
       float diff = max(dot(normal, lightDir), 0.0);
       // specular shading
       vec3 reflectDir = reflect(-lightDir, normal);
       float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
       // attenuation
       float distance = length(light.position - fragPos);
       float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
       // spotlight intensity
       float theta = dot(lightDir, normalize(-light.direction));
       float epsilon = light.cutOff - light.outerCutOff;
       float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
       // combine results
       vec3 ambient = light.ambient * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
       vec3 diffuse = light.diffuse * diff * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
       vec3 specular = light.specular * spec * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
       ambient *= attenuation * intensity;
       diffuse *= attenuation * intensity;
       specular *= attenuation * intensity;
       return (ambient + diffuse + specular);
   }

   void main()
   {
        vec3 normal = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);

                // == =====================================================
                // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
                // For each phase, a calculate function is defined that calculates the corresponding color
                // per lamp. In the main() function we take all the calculated colors and sum them up for
                // this fragment's final color.
                // == =====================================================
                // phase 1: directional lighting
                // phase 2: point lights

        vec3 result = CalcDirLight(dirLight,normal,viewDir);
        result +=CalcSpotLight(spotLight,normal,FragPos,viewDir);

                // phase 3: spot light

        FragColor = vec4(result,1.0);
   }