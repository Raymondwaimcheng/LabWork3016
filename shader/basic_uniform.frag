#version 460

in vec3 Position;
in vec3 Normal;

const float PI = 3.14159265358979323846;

in vec3 LightDir;
in vec3 ViewDir;
flat in vec3 LightIntensity;
in vec2 TexCoord;
in vec3 Vec;

layout (binding = 0) uniform sampler2D ColorTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (binding = 2) uniform samplerCube SkyBoxTex;

layout (location = 0) out vec4 FragColor;

//JJ Lab
uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
}Light;

uniform struct MaterialInfo{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;

}Material;

//Seb Lab
uniform struct PBRLightInfo{
    vec4 Position;
    vec3 L;
}PBRLight[3];

uniform struct PBRMaterialInfo{
    float Rough;
    bool Metal;
    vec3 Color;

}PBRMaterial;

float ggxDistribution(float nDotH){
    float alpha2 = PBRMaterial.Rough * PBRMaterial.Rough * PBRMaterial.Rough * PBRMaterial.Rough;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

float geomSmith(float dotProd){
    float k =(PBRMaterial.Rough + 1.0) * (PBRMaterial.Rough + 1.0) / 8.0;
    float denom = dotProd * (1 - k) + k;
    return 1.0 / denom;
}

vec3 schlickFresnel(float lDotH){
    vec3 f0 = vec3(0.04);
    if(PBRMaterial.Metal){
        f0 = PBRMaterial.Color;
    }
    return f0 + (1 - f0) * pow(1.0 - lDotH, 5);
}

vec3 microfacetModel(int lightIdx, vec3 position, vec3 n){
    vec3 diffuseBrdf = vec3(0.0);
    if(!PBRMaterial.Metal){
        diffuseBrdf = PBRMaterial.Color;
    }

    vec3 l = vec3(0.0), lightI = PBRLight[lightIdx].L;
    //Directional PBRLight
    if(PBRLight[lightIdx].Position.w == 0.0){ 
        l = normalize(PBRLight[lightIdx].Position.xyz);
    }
    //Positional PBRLight
    else{
        l = PBRLight[lightIdx].Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position); //surface normal
    vec3 h = normalize(v + l);
    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBrdf = 0.25 * ggxDistribution(nDotH) * schlickFresnel(lDotH) * geomSmith(nDotV);

    return (diffuseBrdf + PI * specBrdf) * lightI * nDotH;
}

//Fog
uniform struct FogInfo{
    float MaxDist;
    float MinDist;
    vec3 Color;
}Fog;

vec3 blinnPhong(vec3 position, vec3 n){
    vec3 diffuse = vec3(0), spec=vec3(0);

    //Multi Texture
    vec3 texColor = texture(ColorTex, TexCoord).rgb;

    //vec3 ambient = Light.La * Material.Ka;
    vec3 ambient = Light.La * texColor;
    //vec3 s = normalize(Light.Position.xyz - position);
    vec3 s = normalize(LightDir);
    float sDotN = max(dot(s, n), 0.0);
    diffuse = texColor * sDotN;

    if(sDotN > 0.0){
        vec3 v = normalize(-position.xyz);
        //vec3 v = normalize(ViewDir);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }

    return ambient + (diffuse + spec) * Light.L;
}



void main() {
    vec3 NormalLocal = Normal;


    vec3 skyBoxTexColor = texture(SkyBoxTex, normalize(Vec)).rgb;
    NormalLocal = texture(NormalMapTex, TexCoord).xyz;
    NormalLocal.xy = 2.0 * NormalLocal.xy - 1.0;

    //Fog
    float dist = abs(Position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 shadeColor = blinnPhong(Position, normalize(NormalLocal));
    vec3 color = mix(Fog.Color, shadeColor, fogFactor);
    
    //PBR
    vec3 sum = vec3(0);
    vec3 n = normalize(NormalLocal); //surface normal //vec3 n = normalize(Normal)
    for (int i = 0; i < 3; i++){
        sum += microfacetModel(i, Position, n);
    }

    sum = pow(sum, vec3(1.0/2.2));

    //BPR + Fog + Skybox
    FragColor = vec4(sum, 1.0) + vec4(color, 1.0) + vec4(skyBoxTexColor, 1.0);

}
