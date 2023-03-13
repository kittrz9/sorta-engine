#version 330
in vec2 texCoord;
in vec4 color;
uniform sampler2D inputTexture;
uniform vec2 windowSize;
out vec4 fragColor;


void main() {
	fragColor = texture(inputTexture, texCoord) * color;
}
