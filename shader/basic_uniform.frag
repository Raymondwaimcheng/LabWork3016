#version 460

//in vec3 Position;
//in vec3 Normal;
in vec3 LightDir;
in vec3 ViewDir;
//flat in vec3 LightIntensity;
in vec2 TexCoord;

layout (binding = 0) uniform sampler2D ColorTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (location = 0) out vec4 FragColor;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
    //vec3 Direction;
    //float Exponent;
    //float Cutoff;

}Light;

uniform struct MaterialInfo{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;

}Material;

//Fog
uniform struct FogInfo{
    float MaxDist;
    float MinDist;
    vec3 Color;
}Fog;

//const int levels = 10;
//const float scaleFactor = 1.0/levels;

vec3 blinnPhong(vec3 n){
    vec3 diffuse = vec3(0), spec=vec3(0);

    //Multi Texture
    vec3 texColor = texture(ColorTex, TexCoord).rgb;

    vec3 ambient = Light.La * texColor;
    //vec3 s = normalize(Light.Position.xyz - position);
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = texColor * sDotN;

    if(sDotN > 0.0){
        //vec3 v = normalize(-position.xyz);
        vec3 v = normalize(ViewDir);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }

    return ambient +(diffuse + spec) * Light.L;
}

void main() {

    //Fog
    /*float dist=abs(Position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 shadeColor = blinnPhong(Position, normalize(Normal));
    vec3 color = mix(Fog.Color, shadeColor, fogFactor);*/
    

    //Alpha Discard
    /*vec4 alphaMap = texture(AlphaTex, TexCoord);

    if(alphaMap.a < 0.15f){
        discard;
    }
    else{
        if(gl_FrontFacing){
            FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1.0);
        }
        else{
            FragColor = vec4(blinnPhong(Position, normalize(-Normal)), 1.0);
        }
    }*/

    vec3 norm = texture(NormalMapTex, TexCoord).xyz;
    norm.xy = 2.0 * norm.xy - 1.0;

    //Fog
    //FragColor = vec4(color, 1.0);
    //Normal
    //FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1.0);
    //Normal Map
    FragColor = vec4(blinnPhong(normalize(norm)), 1.0);
}
