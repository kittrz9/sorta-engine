#version 330
layout (location = 0) in vec2 vp;
layout (location = 1) in vec2 textureCoords;
layout (location = 2) in vec4 inColor;

out vec2 texCoord;
out vec4 color;

void main() {
	gl_Position = vec4(vp, 1.0, 1.0);
	
	
	texCoord = textureCoords;
	color = inColor;
}
