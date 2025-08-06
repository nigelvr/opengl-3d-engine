#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 outNormal;

uniform vec3 colorOverride;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	if (colorOverride != vec3(0.0f, 0.0f, 0.0f)) {
		FragColor = vec4(colorOverride, 1.0f);
	} else {
		// ambient
		vec3 ambient = 0.4f * lightColor;

		// diffuse
		vec3 lightDir = normalize(lightPos - fragPos);
		float diff = max(dot(lightDir, outNormal), 0.0f);
		vec3 diffuse = diff * lightColor;

		// color
		vec3 color;
		color = (ambient+diffuse)*objectColor;

		FragColor = vec4(color, 1.0f);
	}
}