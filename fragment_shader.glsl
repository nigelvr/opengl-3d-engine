#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 outNormal;

#define MAX_LIGHTS 16
uniform vec3 lightSources[MAX_LIGHTS];

uniform int numLights;
uniform bool isLamp;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	if (isLamp) {
		FragColor = vec4(vec3(1.0f, 1.0f, 1.0f), 1.0f);
	} else {
		// ambient
		vec3 ambient = 0.1f * lightColor;

		float diff = 0.0f;
		for (int i = 0; i < numLights; i++) {
			vec3 lightDir = normalize(lightSources[i] - fragPos);
			diff += max(dot(lightDir, outNormal), 0.0f);
		}

		// diffuse
		//vec3 lightDir = normalize(lightPos - fragPos);
		//float diff = max(dot(lightDir, outNormal), 0.0f);
		vec3 diffuse = diff * lightColor;

		// color
		vec3 color;
		color = (ambient+diffuse)*objectColor;

		FragColor = vec4(color, 1.0f);
	}
}