#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightDir2;
uniform vec3 lightColor2;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform float ambientStrength;
uniform float ambientStrength2;
uniform float specularStrength;
uniform float specularStrength2;

//components
vec3 ambient;
vec3 diffuse;
vec3 specular;

in vec4 fragPosLightSpace;
uniform sampler2D shadowMap;


float computeShadow()
{

	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;

	return shadow;
}


void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));
	vec3 lightDirN2 = vec3(normalize(view * vec4(lightDir2, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor + ambientStrength2 * lightColor2;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor + max(dot(normalEye, lightDirN2), 0.0f) * lightColor2;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
	vec3 reflectDir2 = reflect(-lightDirN2, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	float specCoeff2 = pow(max(dot(viewDir, reflectDir2), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor + specularStrength2 * specCoeff2 * lightColor2;
}


float computeFog()
{
 vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
 float fogDensity = 0.1f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}


void main() 
{
    computeDirLight();

    //compute final vertex color
	//float shadow = computeShadow();
    //vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
	vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
	
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;
	fColor = colorFromTexture;
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}
