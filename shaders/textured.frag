#version 330 core 
out vec4 FragColor; 
 
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

// Material properties
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool hasDiffuseTexture;
    bool hasSpecularTexture;
    bool hasNormalTexture;
};

// Light properties
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// Texture samplers
uniform sampler2D material.diffuse;
uniform sampler2D material.specular;
uniform sampler2D material.normal;

void main() {
    // NEW: Simple highlighting - check material color first
    if (material.diffuse.r > 0.9 && material.diffuse.g > 0.9 && material.diffuse.b < 0.1) {
        // This is our yellow highlight - output bright yellow directly
        FragColor = vec4(1.0, 1.0, 0.0, 1.0); // Bright yellow
        return; // Skip all lighting calculations
    }
    
    // Normalize the normal vector
    vec3 norm = normalize(Normal);
    
    // Calculate the direction vector between light position and fragment position
    vec3 lightDir = normalize(light.position - FragPos);
    
    // Calculate the diffuse impact by generating its dot product with the normal
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Calculate the view direction vector
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate the reflection direction
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // Calculate the specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Calculate ambient, diffuse and specular components
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    
    // Apply textures if available
    if (material.hasDiffuseTexture) {
        vec4 texColor = texture(material.diffuse, TexCoords);
        diffuse *= texColor.rgb;
    }
    
    if (material.hasSpecularTexture) {
        vec3 specularMap = texture(material.specular, TexCoords).rgb;
        specular *= specularMap;
    }
    
    // Combine all components
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result, 1.0);
}
