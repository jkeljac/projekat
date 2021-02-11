#version 330 core

out vec4 FragColor;

struct SpotLight {
    float ind;
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

struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
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

   uniform sampler2D floor_texture;
   uniform vec3 viewPos;
   uniform SpotLight spotLight;
   uniform DirLight dirLight;
   uniform PointLight pointLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

    vec3 ambient = light.ambient * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
    vec3 diffuse = light.diffuse * diff * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
    vec3 specular = light.specular * spec * vec3(texture(floor_texture, TexCoords)*vec4(aColor,1.0));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0f);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(floor_texture,  TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(floor_texture,  TexCoords));
    vec3 specular = light.specular * spec/3 * vec3(texture(floor_texture,  TexCoords).xxx);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
 {
     vec3 lightDir = normalize(light.position - fragPos);
     float diff = max(dot(normal, lightDir), 0.0);

     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

     float distance = length(light.position - fragPos);
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

     float theta = dot(lightDir, normalize(-light.direction));
     float epsilon = light.cutOff - light.outerCutOff;
     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

     vec3 ambient = light.ambient * vec3(texture(floor_texture, TexCoords));
     vec3 diffuse = light.diffuse * diff * vec3(texture(floor_texture, TexCoords));
     vec3 specular = light.specular * spec * vec3(texture(floor_texture, TexCoords));
     ambient *= attenuation * intensity;
     diffuse *= attenuation * intensity;
     specular *= attenuation * intensity;
     return (ambient + diffuse + specular);
 }
   void main()
   {
        vec3 normal = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 result = CalcDirLight(dirLight,normal,viewDir);
        result += CalcSpotLight(spotLight,normal,FragPos,viewDir)*spotLight.ind;
        result += CalcPointLight(pointLight,normal,FragPos,viewDir);
        FragColor = vec4(result,1.0);
   }