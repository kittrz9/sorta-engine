#version 330

uniform vec2 windowSize;
uniform float time;

out vec4 fragColor;

const float lineWidth = 0.2;

const float pi = 3.1415926535;

void main() {
	vec2 pos;
	pos.x = pi * gl_FragCoord.x / windowSize.x * (gl_FragCoord.y / windowSize.y);
	pos.y = pi * gl_FragCoord.y / windowSize.y * 1.2;
	float value = (sin(time + (pos.x + pos.y) / lineWidth) + sin(time + 2.0*(pos.x - pos.y) / lineWidth));
	if(value > 0) {
		fragColor = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	} else {
		fragColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);
	}
}
