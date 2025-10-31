#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

#define MAX_LIGHTS 16
uniform vec3 lightSources[MAX_LIGHTS];
uniform int numLights;
uniform vec3 cameraPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

float diffuse_light(int light_index) {
	vec3 lightDir = normalize(lightSources[light_index] - FragPos);
	return max(dot(lightDir, Normal), 0.0f);
}

float spec_light(int light_index) {
	vec3 lightDir = normalize(lightSources[light_index] - FragPos);
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, Normal);
	return pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
}

vec4 getColor() {
	float ambient_strength = 0.3f;
	float specularStrength = 0.5;
	float diff = 0.0f;
	float spec = 0.0f;
	for (int i = 0; i < numLights; i++) {
		diff += diffuse_light(i);
		spec += spec_light(i);
	}

	return vec4((ambient_strength + diff + specularStrength*spec)*lightColor*objectColor, 1.0f);
}

void main()
{
	FragColor = getColor();
}