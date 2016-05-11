#version 330 core

//材质
layout(std140) uniform Material
{
    vec4  ambientColor;
    vec4  diffuseColor;
    vec4  specularColor;
    float shininess;
    bool  ambientTexed;
    bool  diffuseTexed;
    bool  specularTexed;
    bool  heightTexed;
    bool  normalTexed;
    bool  alphaTexed;
//    float bumpScale;
} material;

uniform sampler2D ambientTex;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D bumpTex;
uniform sampler2D alphaTex;

//光照
struct Light
{
    int  type;
    vec3 location;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuate;
    vec2 cutoff;
    bool enabled;
};
uniform Light lightDirect, lightSpot;

in vec2 fTexCoord;
in mat3 TBN;
in vec3 tanFragPos;
in vec3 tanViewPos;
in vec3 tanNormal;

out vec4 fragColor;


const vec2 constantList = vec2(1.0, 0.0);   //用于实现快速的MAD运算
const vec4 allInOnes = vec4(1.0);

//计算光照
vec4 calcLight(Light lit)
{
    //关灯
    if (!lit.enabled)
        return vec4(0.0);
    
    vec3 tanLightPos = TBN * lit.location;
    vec3 tanLightDir = TBN * lit.direction;
    //根据有无贴图取得颜色
    vec4 matAmbient, matDiffuse, matSpecular;
    vec3 matBump;
    float matAlpha;
    
    if (material.ambientTexed) //有漫射贴图
        matAmbient = texture(ambientTex, fTexCoord);
    else if (material.diffuseTexed)    //无漫射但有散射贴图
        matAmbient = texture(diffuseTex, fTexCoord);
    else    //无漫射贴图
        matAmbient = material.ambientColor;
    
    if (material.diffuseTexed) //有散射贴图
        matDiffuse = texture(diffuseTex, fTexCoord);
    else
        matDiffuse = material.diffuseColor;
    
    if (material.specularTexed) //有高光贴图
        matSpecular = texture(specularTex, fTexCoord);
    else
        matSpecular = material.specularColor;
    
    if (material.normalTexed)  //有法线贴图
        matBump = texture(bumpTex, fTexCoord).xyz * 2.0 - 1.0;
    else if (material.heightTexed)  //高度贴图，计算法线
    {
        vec3 bumpCenter = texture(bumpTex, fTexCoord).xyz;
        float bumpRight = textureOffset(bumpTex, fTexCoord, ivec2(1, 0)).x;
        float bumpUp = textureOffset(bumpTex, fTexCoord, ivec2(0, -1)).x;
        matBump = bumpCenter.xxx * constantList.xxy - vec3(bumpRight, bumpUp, -1.0);
    }
    
    if (material.alphaTexed)    //透明贴图
        matAlpha = texture(alphaTex, fTexCoord).r;  //只取红通道
    
    //漫反射颜色
    vec4 ambient = matAmbient * lit.ambient.rgbb;

    //散射颜色
    vec3 norm;
    if (material.heightTexed || material.normalTexed)
        norm = normalize(matBump);
    else
        norm = normalize(tanNormal);
    vec3 lightDir;
    if (lit.type == 0)
        lightDir = normalize(tanLightDir);
    else
        lightDir = normalize(tanLightPos - tanFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * matDiffuse * lit.diffuse.rgbb;

    //镜面反射颜色
    vec3 viewDir = normalize(tanViewPos - tanFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, norm);
    float spec;
    if (material.shininess > 0)  //假定镜面系数=0时无反射（规避一些问题）
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    else
        spec = 0;
    vec4 specular = spec * matSpecular * lit.specular.rgbb;

    //衰减因子
    float attenuate;
    if (lit.type == 0)
        attenuate = 1.0;
    else
    {
        float dist = length(tanLightPos - tanFragPos);
        attenuate = 1.0 / (lit.attenuate.x + lit.attenuate.y * dist + lit.attenuate.z * dist * dist);
    }
    //软边缘
    float cutoff;
    if (lit.type == 2)
    {
        float theta = dot(lightDir, normalize(tanLightDir));
        float epsilon = lit.cutoff.x - lit.cutoff.y;
        cutoff = clamp((theta - lit.cutoff.y) / epsilon, 0.0, 1.0);
    }
    else
        cutoff = 1.0;
    //透明度
    vec4 alpha = constantList.yyyx;
    if (material.alphaTexed)
        alpha.a = matAlpha;
    if (matDiffuse.a < alpha.a)
        alpha.a = matDiffuse.a;
    
    vec4 final = (ambient + (diffuse + specular) * cutoff) * attenuate;
    //只考虑散射贴图和透明贴图的透明度
    return final.rgba * constantList.xxxy + alpha;
}

//忽略光照，只算散射
vec4 calcDiffuse()
{
    vec4 matDiffuse;
    if (material.diffuseTexed) //有散射贴图
        matDiffuse = texture(diffuseTex, fTexCoord);
    else
        matDiffuse = material.diffuseColor;
    
    return matDiffuse;
}

void main()
{
    vec4 final = calcLight(lightDirect);
    vec4 alpha = constantList.yyyx;
    alpha.a = final.a;

    final += calcLight(lightSpot);
//    vec4 final = calcDiffuse();
    
    fragColor = final.rgba * constantList.xxxy + alpha;
}
