#version 330

in vec2 texCoord;
in vec4 color;
uniform sampler2D inputTexture;
uniform vec2 windowSize;

// https://github.com/Chlumsky/msdfgen

float median(float r, float g, float b) {
	return max(min(r,g), min(max(r,g),b));
}

float screenPxRange() {
	float pxRange = 250.0f;
	vec2 unitRange = vec2(pxRange)/vec2(textureSize(inputTexture,0));
	vec2 screenTexSize = vec2(1.0)/fwidth(texCoord);
	return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

out vec4 fragColor;
void main() {
	ivec2 texSize = textureSize(inputTexture, 0);
	vec2 texCoord2 = texCoord / texSize;
	vec3 msd = texture(inputTexture, texCoord2).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange()*(sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	fragColor = color * vec4(1.0,1.0,1.0,opacity);
}
