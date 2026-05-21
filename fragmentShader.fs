#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

// Material Properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    vec3 emissive;   // NEW: Emissive Color
};
uniform Material material;

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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

// 3. Spot Light (NEW)

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
uniform bool spotLightOn; // NEW: Key 3 Toggle


uniform bool useObjectColor; // The toggle
uniform vec3 objectColor;    // The color (e.g., Red for X-axis)
uniform float alpha;         // Opacity

// Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{
    // ========================================================
    // 1. SIMPLE COLOR MODE (For Axes, Debugging, etc.)
    // ========================================================
    if (useObjectColor)
    {
        // Directly output the color passed from C++ without lighting calculations
        FragColor = vec4(objectColor, alpha);
        return; // Exit main() early
    }

    // ========================================================
    // 2. STANDARD LIGHTING MODE (For 3D Objects)
    // ========================================================
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    // Directional Light
    if(directLightOn) {
        result += CalcDirLight(dirLight, norm, viewDir);
    }

    // Point Lights
    if(pointLightOn) {
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    // Spot Light (Flashlight)
    if(spotLightOn) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    }
    
    // Add Emissive Light
    result += material.emissive;

    // Final output for lit objects
    FragColor = vec4(result, 1.0);
}

// ... [CalcDirLight and CalcPointLight functions remain the same] ...
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
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
    
    vec3 ambient  = light.ambient  * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    if(!ambientOn) ambient = vec3(0.0);
    if(!diffuseOn) diffuse = vec3(0.0);
    if(!specularOn) specular = vec3(0.0);
    
    return (ambient + diffuse + specular);
}

/// ==========================================
// Spot Light Function (NEW)
// ==========================================
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Check if lighting is inside the spotlight cone
    // theta is the cosine of the angle between light direction and spot direction
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Ambient is always applied (as per theory snippet "else use ambient")
    vec3 ambient = light.ambient * material.ambient;
    ambient *= attenuation;
    
    if(!ambientOn) ambient = vec3(0.0);

    if(theta > light.cutOff) // Inside the cone
    {
        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * material.diffuse;
        
        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * material.specular;
        
        diffuse *= attenuation;
        specular *= attenuation;
        
        // Toggles
        if(!diffuseOn) diffuse = vec3(0.0);
        if(!specularOn) specular = vec3(0.0);
        
        return (ambient + diffuse + specular);
    }
    else
    {
        // Outside the cone, just ambient
        return ambient;
    }
}