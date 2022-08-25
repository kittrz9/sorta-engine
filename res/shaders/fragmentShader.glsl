#version 400
in vec2 texCoord;
uniform vec4 inputColor;
uniform bool useTexture;
uniform sampler2D inputTexture;
uniform vec4 textureRect;
uniform vec2 windowSize;
out vec4 fragColor;
void main() {
	if(useTexture){
		ivec2 texSize = textureSize(inputTexture,0);
		vec2 texCoord2 = vec2((texCoord.x * (textureRect.z/texSize.x)) + (textureRect.x/texSize.x), (texCoord.y * (textureRect.w/texSize.y)) + (textureRect.y/texSize.y));
		fragColor = texture(inputTexture, texCoord2) * inputColor;
	} else {
		fragColor = inputColor;
	}
}
