#version 400
in vec2 texCoord;
in vec4 color;
uniform sampler2D inputTexture;
uniform vec2 windowSize;
out vec4 fragColor;


void main() {
	fragColor = texture(inputTexture, texCoord) * color * (vec4(vec3(1-gl_FragCoord.x/(windowSize.x/4)),1.0));
}
