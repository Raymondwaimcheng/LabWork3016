#version 460

layout (location = 0) in vec3 VertexInitVel;
layout (location = 1) in float VertexBirthTime;

out float Transp;
out vec2 ParticleTexCoord;

uniform float ParticleTime;

uniform vec3 ParticleGravity = vec3(0.0, -0.05, 0.0);
uniform float ParticleLifeTime;
uniform float ParticleSize = 1.0;
uniform vec3 EmitterPos;

uniform mat4 ModelViewMatrix; //MV
uniform mat4 projection;

const vec3 offsets[] = vec3[](vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0), vec3(0.5, 0.5, 0), vec3(-0.5, -0.5, 0), vec3(0.5, 0.5, 0), vec3(-0.5, 0.5, 0));
const vec2 texCoords[] = vec2[](vec2(0,0), vec2(1,0), vec2(1,1), vec2(0,0), vec2(1,1), vec2(0,1));

void main(){
	
	vec3 cameraPos;

	float t = ParticleTime - VertexBirthTime;
	
	if( t >= 0 && t < ParticleLifeTime){
		vec3 pos = EmitterPos + VertexInitVel * t + ParticleGravity * t * t;
		cameraPos = (ModelViewMatrix * vec4(pos, 1)).xyz + (offsets[gl_VertexID] * ParticleSize);
		Transp = mix(1, 0, t / ParticleLifeTime);
	}
	//If Particle Doesn't exist
	else{
		cameraPos = vec3(0);
		Transp = 0.0;

	}

	ParticleTexCoord = texCoords[gl_VertexID];
	gl_Position = projection * vec4(cameraPos, 1);
}
