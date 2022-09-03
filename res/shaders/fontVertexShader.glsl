#version 400
layout (location = 0) in vec2 vp;
layout (location = 1) in vec2 textureCoords;

out vec2 texCoord;
uniform vec2 windowSize;

vec3 scalePoint(vec3 input, vec2 scaling){
	return vec3(input.x * scaling.x, input.y * scaling.y, 1.0f);
}

vec3 rotatePoint(vec3 input, float angle2){
	return vec3((cos(angle2)*input.x)+(sin(angle2)*input.y), (cos(angle2)*input.y)-(sin(angle2)*input.x), input.z);
}

vec3 translatePoint(vec3 input, vec2 translation){
	return vec3(input.x + translation.x, input.y + translation.y, 1.0);
}

void main() {
	gl_Position = vec4(vp, 1.0, 1.0);

	texCoord = textureCoords;
}