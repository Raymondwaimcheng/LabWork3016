#version 460

in float Transp;
in vec2 ParticleTexCoord;

uniform sampler2D ParticleTexture;

layout(location = 0) out vec4 FragColor;

void main(){

    FragColor = texture(ParticleTexture, ParticleTexCoord);
    //FragColor = vec4(mix(vec3(0,0,0), FragColor.xyz), FragColor.a);
    FragColor.a *= Transp;
}
