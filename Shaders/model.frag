#version 330 core

//材质
struct Material
{
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
    bvec3 textured;
    sampler2D ambientTex;
    sampler2D diffuseTex;
    sampler2D specularTex;
};
uniform Material material;

//光照
struct Light
{
    int type;
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

uniform vec3 viewPos;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fragPos;

out vec4 fragColor;

//计算光照
vec4 calcLight(Material mat, Light lit)
{
    //关灯
    if (!lit.enabled)
        return vec4(0.0);
    
    //根据有无纹理取得颜色
    vec4 matAmbient, matDiffuse, matSpecular;
    if (mat.textured.x)
        matAmbient = texture(mat.ambientTex, fTexCoord);
    else
        matAmbient = vec4(mat.ambientColor, 1.0f);
    if (mat.textured.y)
        matDiffuse = texture(mat.diffuseTex, fTexCoord);
    else
        matDiffuse = vec4(mat.diffuseColor, 1.0f);
    if (mat.textured.z)
        matSpecular = texture(mat.specularTex, fTexCoord);
    else
        matSpecular = vec4(mat.specularColor, 1.0f);
    
    //漫反射颜色
    vec4 ambient = matAmbient * vec4(lit.ambient, 1.0f);
    //散射颜色
    vec3 norm = normalize(fNormal);
    vec3 lightDir;
    if (lit.type == 0)
        lightDir = normalize(lit.direction);
    else
        lightDir = normalize(lit.location - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * matDiffuse * vec4(lit.diffuse, 1.0f);
    //镜面反射颜色
    vec3 viewDir = normalize(viewPos - fragPos);
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
        float dist = length(lit.location - fragPos);
        attenuate = 1.0 / (lit.attenuate.x
                           + lit.attenuate.y * dist
                           + lit.attenuate.z * dist * dist);
    }
    //软边缘
    float cutoff;
    if (lit.type == 2)
    {
        float theta = dot(lightDir, normalize(lit.direction));
        float epsilon = lit.cutoff.x - lit.cutoff.y;
        cutoff = clamp((theta - lit.cutoff.y) / epsilon, 0.0, 1.0);
    }
    else
        cutoff = 1.0;
    
    return (ambient + (diffuse + specular) * cutoff) * attenuate;
}

void main()
{
    vec4 final = calcLight(material, lightDirect);
    final += calcLight(material, lightSpot);
    
    fragColor = final;
}
