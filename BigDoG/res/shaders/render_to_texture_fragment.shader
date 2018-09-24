#version 330 core

in vec2 UV;

layout(location = 0) out vec4 color;

uniform sampler2D dataTex;
uniform sampler2D filterTex;

void main()
{
	vec4 dataColor = texture(dataTex, UV);
	int numOfValues = int(texture(filterTex, vec2(0.0, 0.0)));
	float colorVal = 1.0;
	for (int i = 0; i < numOfValues; i++) {
		float value = int(texture(filterTex, vec2(((i+1)/2048.0), 0.0)));
		if (dataColor.x < value) {
			colorVal = 0.0;
			break;
		}
	}
	color = vec4(colorVal, colorVal, colorVal, 1.0);
}