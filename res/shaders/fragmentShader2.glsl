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
		// I think I might have messed up the windowSize uniform since I can't get it to work with this so this is hard coded and wont work when the window is resized
		fragColor = texture(inputTexture, texCoord2) * inputColor * (vec4(vec3(1-gl_FragCoord.x/(windowSize.x/4)),1.0));
	} else {
		fragColor = inputColor;
	}
}
