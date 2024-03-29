#version 330

// In 
in vec4 vColor;	
in vec2 TexCoord;	
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

// Out
out vec4 colour;

// Const
const int MAX_POINT_LIGHTS = 32;
const int MAX_SPOT_LIGHTS = 32;

// Structs
struct Light {

	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;

};

struct DirectionalLight {

	Light base;
	vec3 direction;

};

struct PointLight {

	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;

};

struct SpotLight {

	PointLight base;
	vec3 direction;
	float edge;


};

struct OmniShadowMap {

	samplerCube shadowMap;
	float farPlane;

};

struct Material {

	float specularIntensity;
	float shineness;

};

// Lights
uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
uniform Material material;

uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform vec3 eyePosition; // Camera position

// OFFSET directions to help calculate point shadow 
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

vec4 CalcLightByDirection (Light light, vec3 direction, float shadowFactor){

	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;	
	
	// A . B = |A| x |B| x cos(angle)
	//       =  1  x  1  x cos(angle)
	//       = cos(angle)                     max keep it positive (no negative light lol)
	float diffuseFactor = max(dot(normalize(Normal) , normalize(direction)), 0.0f);
	vec4 diffuseColour  = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);

	vec4 specularColour = vec4(0, 0, 0, 0);

	if ( diffuseFactor > 0.0f ) {

		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));

		float specularFactor = dot(fragToEye, reflectedVertex); // Give us the Cossine

		if ( specularFactor > 0.0f ) {

			specularFactor = pow(specularFactor, material.shineness);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);

		}

	}

	return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour));

}

float CalcPointShadowFactor(PointLight light, int shadowIndex) {

	vec3 fragToLight = FragPos - light.position;
	float currentDepth = length(fragToLight);
	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;

	float viewDistance = length(eyePosition - FragPos);
	float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0;

	for(int i =0; i < samples; ++i){

		float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r; // .r -> only first value
		closestDepth *= omniShadowMaps[shadowIndex].farPlane; // because we divided it in another frag

		if(currentDepth - bias > closestDepth) {

			shadow += 1.0;

		}
	}
	
	shadow /= float(samples);
	return shadow;

}

float CalcShadowFactor(vec4 DirectionalLightSpacePos){

	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w ; // W is the forth value
	projCoords = (projCoords * 0.5) + 0.5; // Converto to a number between 0 and 1. nice

	float closestDepth = texture(directionalShadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z; // distance from the light

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(directionalLight.direction);
	float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.000005); // Bias to correct Shadow Acne
	
	float shadow = 0.0;
	vec2 texelsize = 1.0 / textureSize(directionalShadowMap, 0);

	// PCF algorith do smooth the edges of the shadows
	for(int x = -1; x <= 0; x++){

		for(int y = -1; y <= 0; y++){

			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelsize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			
		}

	}

	shadow /= 9.0; // 9 is the default from the algorithm

	if (projCoords.z > 1.0) {

		shadow = 0.0;

	}

	return shadow;

}

vec4 CalcDirectionalLight(vec4 DirectionalLightSpacePos) {

	float shadowFactor = CalcShadowFactor(DirectionalLightSpacePos);

	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);

}

vec4 CalcPointLight(PointLight pLight, int shadowIndex){

	vec3 direction = FragPos - pLight.position;
	float distance = length(direction);
	direction = normalize(direction);

	float shadowFactor = CalcPointShadowFactor(pLight, shadowIndex);

	vec4 colour = CalcLightByDirection(pLight.base, direction, shadowFactor);
	float attenuation = pLight.exponent * distance * distance +
						pLight.linear * distance + 
						pLight.constant;

	return (colour / attenuation);

}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex){

	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if( slFactor > sLight.edge ) {

		vec4 colour = CalcPointLight(sLight.base, shadowIndex);

		//Smooth Edges
		return colour * (1.0f - (1.0f - slFactor) * (1.0f/ (1.0f - sLight.edge)));
		
		// Sharp Edges
		//return colour;

	} else {

		return vec4(0, 0, 0, 0);

	}

}

vec4 CalcPointLights(){

	vec4 totalColour = vec4(0, 0, 0, 0);

	for(int i = 0; i < pointLightCount; i++){

		totalColour += CalcPointLight(pointLights[i], i);

	}

	return totalColour;
}

vec4 CalcSpotLights() {

	vec4 totalColour = vec4(0, 0, 0, 0);

	for(int i = 0; i < spotLightCount; i++){

		totalColour += CalcSpotLight(spotLights[i], i + pointLightCount);

	}

	return totalColour;

}


void main() {	

	vec4 finalColour = CalcDirectionalLight(DirectionalLightSpacePos);
	finalColour += CalcPointLights();
	finalColour += CalcSpotLights();

	colour = texture(theTexture, TexCoord) * finalColour;

}