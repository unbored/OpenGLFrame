#version 330 core

//材质
struct Material
{
    vec3  ambientColor;
    vec3  diffuseColor;
    vec3  specularColor;
    float shininess;
    bool  ambientTexed;
    bool  diffuseTexed;
    bool  specularTexed;
    bool  bumpTexed;
    bool  alphaTexed;
    sampler2D ambientTex;
    sampler2D diffuseTex;
    sampler2D specularTex;
    sampler2D bumpTex;
    float bumpScale;
    sampler2D alphaTex;
};
uniform Material material;

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

in vec3 fNormal;
in vec2 fTexCoord;
in mat3 TBN;
in vec3 tanFragPos;
in vec3 tanViewPos;

out vec4 fragColor;

//计算光照
vec4 calcLight(Material mat, Light lit)
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
    
    if (mat.ambientTexed) //有漫射贴图
        matAmbient = texture(mat.ambientTex, fTexCoord);
    else if (mat.diffuseTexed)    //无漫射但有散射贴图
        matAmbient = texture(mat.diffuseTex, fTexCoord);
    else    //无漫射贴图
        matAmbient = vec4(mat.ambientColor, 1.0f);
    
    if (mat.diffuseTexed) //有散射贴图
        matDiffuse = texture(mat.diffuseTex, fTexCoord);
    else
        matDiffuse = vec4(mat.diffuseColor, 1.0f);
    
    if (mat.specularTexed) //有高光贴图
        matSpecular = texture(mat.specularTex, fTexCoord);
    else
        matSpecular = vec4(mat.specularColor, 1.0f);
    
    if (mat.bumpTexed)  //有法线贴图
        matBump = texture(mat.bumpTex, fTexCoord).xyz;
    
    if (mat.alphaTexed)
        matAlpha = texture(mat.alphaTex, fTexCoord).r;
    
    //漫反射颜色
    vec4 ambient = matAmbient * vec4(lit.ambient, 1.0f);
    //散射颜色
    vec3 norm;
    if (mat.bumpTexed)
    {
        vec3 matNorm = matBump * 2 - vec3(1.0);
        norm = normalize(matNorm);
    }
    else
        norm = normalize(TBN * fNormal);
    vec3 lightDir;
    if (lit.type == 0)
        lightDir = normalize(tanLightDir);
    else
        lightDir = normalize(tanLightPos - tanFragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * matDiffuse * vec4(lit.diffuse, 1.0f);
    //镜面反射颜色
    vec3 viewDir = normalize(tanViewPos - tanFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, norm);
    float spec;
    if (mat.shininess > 0)  //假定镜面系数=0时无反射（规避一些问题）
        spec = pow(max(dot(norm, halfwayDir), 0.0), mat.shininess);
    else
        spec = 0;
    vec4 specular = spec * matSpecular * vec4(lit.specular, 1.0f);
    //衰减因子
    float attenuate;
    if (lit.type == 0)
        attenuate = 1.0;
    else
    {
        float dist = length(tanLightPos - tanFragPos);
        attenuate = 1.0 / (lit.attenuate.x
                           + lit.attenuate.y * dist
                           + lit.attenuate.z * dist * dist);
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
    vec4 alpha = vec4(1.0);
    if (mat.alphaTexed)
        alpha.a = matAlpha;
    
    return (ambient + (diffuse + specular) * cutoff) * attenuate * alpha;
}

//忽略光照，只算散射
//vec4 calcDiffuse(Material mat)
//{
//    vec4 matDiffuse;
//    if (mat.diffuseTexed) //有散射贴图
//        matDiffuse = texture(mat.diffuseTex, fTexCoord);
//    else
//        matDiffuse = vec4(mat.diffuseColor, 1.0f);
//    
//    return matDiffuse;
//}

void main()
{
    vec4 final = calcLight(material, lightDirect);
    final += calcLight(material, lightSpot);
    
    fragColor = final;
}
