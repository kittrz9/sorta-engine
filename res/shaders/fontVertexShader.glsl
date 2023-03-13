#version 330
layout (location = 0) in vec2 vp;
layout (location = 1) in vec2 textureCoords;

out vec2 texCoord;
uniform vec2 windowSize;

void main() {
	gl_Position = vec4(vp, 1.0, 1.0);

	texCoord = textureCoords;
}
