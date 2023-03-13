#version 330
layout (location = 0) in vec2 vp;
layout (location = 1) in vec2 textureCoords;

out vec2 texCoord;
uniform vec2 windowSize;
uniform float time;

vec3 scalePoint(vec3 inputVec, vec2 scaling){
	return vec3(inputVec.x * scaling.x, inputVec.y * scaling.y, 1.0f);
}

vec3 rotatePoint(vec3 inputVec, float angle2){
	return vec3((cos(angle2)*inputVec.x)+(sin(angle2)*inputVec.y), (cos(angle2)*inputVec.y)-(sin(angle2)*inputVec.x), inputVec.z);
}

vec3 translatePoint(vec3 inputVec, vec2 translation){
	return vec3(inputVec.x + translation.x, inputVec.y + translation.y, 1.0);
}

void main() {
	gl_Position = vec4(vp, 1.0, 1.0);
	gl_Position.xyz = translatePoint(gl_Position.xyz, 0.02*rotatePoint(vec3(1.0), time*3.14159*2 + vp.x*10 + vp.y*10).xy);

	texCoord = textureCoords;
}
