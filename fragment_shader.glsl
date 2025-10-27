#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

#define MAX_LIGHTS 16
uniform vec3 lightSources[MAX_LIGHTS];
uniform int numLights;

uniform bool isLamp;
uniform vec3 cameraPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

vec4 getColor() {
	// ambient
	vec3 ambient = 0.3f * lightColor;
	float specularStrength = 0.5;

	float diff = 0.0f;
	float spec = 0.0f;
	for (int i = 0; i < numLights; i++) {
		vec3 lightDir = normalize(lightSources[i] - FragPos);
		diff += max(dot(lightDir, Normal), 0.0f);

		vec3 viewDir = normalize(cameraPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, Normal);
		spec += pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	}

	// diffuse
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 specular = specularStrength * spec * lightColor;

	// color
	vec3 color = (ambient+diffuse+specular)*objectColor;

	return vec4(color, 1.0f);
}

void main()
{
	FragColor = getColor();
}