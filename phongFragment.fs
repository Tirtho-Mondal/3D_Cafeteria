#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos;

// Material Properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    vec3 emissive;
};
uniform Material material;

// Texture
uniform sampler2D texture1;
uniform bool useTexture;
uniform int renderMode;    // 0=material, 1=texture, 2=blended
uniform vec2 uvTiling;

// 1. Directional Light
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

// 2. Point Light
struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 5
uniform PointLight pointLights[NR_POINT_LIGHTS];

// 3. Spot Light
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform SpotLight spotLight;

// Toggles
uniform bool directLightOn;
uniform bool ambientOn;
uniform bool diffuseOn;
uniform bool specularOn;
uniform bool pointLightOn;
uniform bool spotLightOn;

uniform bool useObjectColor;
uniform vec3 objectColor;
uniform float alpha;

// Get effective ambient and diffuse colors based on renderMode
vec3 getEffectiveAmbient()
{
    if (!useTexture) return material.ambient;

    vec3 texColor = texture(texture1, TexCoord * uvTiling).rgb;
    if (renderMode == 0) return material.ambient;
    if (renderMode == 1) return texColor * 0.4;
    return mix(material.ambient, texColor * 0.4, 0.5); // blended
}

vec3 getEffectiveDiffuse()
{
    if (!useTexture) return material.diffuse;

    vec3 texColor = texture(texture1, TexCoord * uvTiling).rgb;
    if (renderMode == 0) return material.diffuse;
    if (renderMode == 1) return texColor;
    return mix(material.diffuse, texColor, 0.5); // blended
}

// Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform vec3 globalAmbient; // Global ambient light color


void main()
{
    // Simple color mode (axes, debugging)
    if (useObjectColor)
    {
        FragColor = vec4(objectColor, alpha);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = globalAmbient * getEffectiveAmbient();

    // Directional Light
    if(directLightOn) {
        result += CalcDirLight(dirLight, norm, viewDir);
    }

    // Point Lights
    if(pointLightOn) {
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // Spot Light
    if(spotLightOn) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    }

    // Emissive
    result += material.emissive;

    FragColor = vec4(result, alpha);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * getEffectiveAmbient();
    vec3 diffuse  = light.diffuse  * diff * getEffectiveDiffuse();
    vec3 specular = light.specular * spec * material.specular;

    if(!ambientOn) ambient = vec3(0.0);
    if(!diffuseOn) diffuse = vec3(0.0);
    if(!specularOn) specular = vec3(0.0);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient  = light.ambient  * getEffectiveAmbient();
    vec3 diffuse  = light.diffuse  * diff * getEffectiveDiffuse();
    vec3 specular = light.specular * spec * material.specular;

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    if(!ambientOn) ambient = vec3(0.0);
    if(!diffuseOn) diffuse = vec3(0.0);
    if(!specularOn) specular = vec3(0.0);

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * getEffectiveAmbient();
    ambient *= attenuation;

    if(!ambientOn) ambient = vec3(0.0);

    if(theta > light.cutOff)
    {
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * getEffectiveDiffuse();

        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * material.specular;

        diffuse *= attenuation;
        specular *= attenuation;

        if(!diffuseOn) diffuse = vec3(0.0);
        if(!specularOn) specular = vec3(0.0);

        return (ambient + diffuse + specular);
    }
    else
    {
        return ambient;
    }
}
