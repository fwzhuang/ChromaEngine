#version 330 core
// out
out vec4 FragColor;

// structs
struct Material
{
	//textures
	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_diffuse3;
	sampler2D texture_specular1;
	sampler2D texture_specular2;

	samplerCube skybox;

	float roughness;
	float ambientBrightness;
	float specularIntensity;
};

struct DirLight 
{
	vec3 direction;
	vec3 diffuse;
	float intensity;
};

struct PointLight
{
	// attenuation
	float constant;
	float linear;
	float quadratic;
	// members
	vec3 position;
	vec3 diffuse;
	float intensity;
};

struct SpotLight
{
	// attenuation
	float constant;
	float linear;
	float quadratic;
	// members
	vec3 position;
	vec3 direction;
	float intensity;
	float spotSize;
	float penumbraSize;
	vec3 diffuse;
};

#define NR_POINT_LIGHTS 4
#define NR_DIR_LIGHTS 1
#define NR_SPOT_LIGHTS 1

// in
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// uniforms
uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

// functions prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseMap, vec3 specMap);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 FragPos, vec3 diffuseMap, vec3 specMap);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 FragPos,  vec3 diffuseMap, vec3 specMap);


void main()
{
	vec3 result;
	// maps
	vec3 diffuseMap = vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specularMap = vec3(texture(material.texture_specular1, TexCoords));

	// attrs
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	// direction lights
	for(int i = 0; i < NR_DIR_LIGHTS ; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir,  diffuseMap, specularMap);

	
	// spot lights

	for(int i = 0; i < NR_SPOT_LIGHTS ; i++)
		//result += CalcSpotLight(spotLights[i], norm, viewDir, FragPos, diffuseMap, specularMap);


	// point lights
	for(int i = 0; i < NR_POINT_LIGHTS ; i++)
		result += CalcPointLight(pointLights[i], norm, viewDir, FragPos, diffuseMap, specularMap);


	FragColor = vec4(result, 1.0);

}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseMap, vec3 specMap)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse
	vec3 diffuse = diffuseMap * light.diffuse * max(dot(lightDir, normal), 0.0) * light.intensity;
	// specular 
	vec3 reflectedLight = normalize(reflect(-lightDir, normal));
	vec3 specular = specMap * pow(max(dot(reflectedLight, viewDir), 0.0), material.roughness) * material.specularIntensity * light.intensity;
	// ambient
	vec3 ambient = diffuseMap * material.ambientBrightness * light.intensity;

	return (max(diffuse, ambient) + specular );
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 FragPos,  vec3 diffuseMap, vec3 specMap)
{
	vec3 lightDir = normalize(light.position - FragPos);
	// diffuse
	vec3 diffuse = diffuseMap * light.diffuse * max(dot(lightDir, normal), 0.0);
	// specular 
	vec3 reflectedLight = reflect(-lightDir, normal);
	vec3 specular = specMap * pow(max(dot(reflectedLight, viewDir), 0.0), material.roughness) * material.specularIntensity;
	// ambient
	vec3 ambient = diffuseMap * material.ambientBrightness;
	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	diffuse *=  attenuation * light.intensity;
	specular *= attenuation * light.intensity;
	ambient *= attenuation * light.intensity;

	// return
	return (max(diffuse, ambient) + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 FragPos,  vec3 diffuseMap, vec3 specMap)
{
	// lightDir
	vec3 lightDir = normalize(light.position - FragPos);
	// diffuse
	float theta = dot(lightDir, -viewDir);
	float lit = clamp(1.0 - smoothstep(light.penumbraSize, light.spotSize, theta), 0.0, 1.0);
	vec3 diffuse = diffuseMap * light.diffuse * lit;
	// specular 
	vec3 reflectedLight = normalize(reflect(-lightDir, normal));
	vec3 specular = specMap * pow(max(dot(reflectedLight, viewDir), 0.0), material.roughness) * material.specularIntensity * lit;
	// ambient
	vec3 ambient = diffuseMap * material.ambientBrightness ;
	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	diffuse *= attenuation * light.intensity;
	specular *= attenuation * light.intensity;
	ambient *= attenuation * light.intensity;
	// return
	return (max(diffuse, ambient) + specular);
}