#version 330 core

//材质
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
//uniform Material material;

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

uniform sampler2D tex;
uniform sampler2D tex1;

uniform vec3 viewPos;

in vec2 fTexCoord;
in vec3 fNormal;
in vec3 fragPos;

out vec4 fragColor;

//计算光照
vec3 calcLight(Material mat, Light lit)
{
    if (!lit.enabled)
        return vec3(0.0);
    //漫反射颜色
    vec3 ambient = mat.ambient * lit.ambient;
    //散射颜色
    vec3 norm = normalize(fNormal);
    vec3 lightDir;
    if (lit.type == 0)
        lightDir = normalize(lit.direction);
    else
        lightDir = normalize(lit.location - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * mat.diffuse * lit.diffuse;
    //镜面反射颜色
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), mat.shininess);
    vec3 specular = spec * mat.specular * lit.specular;
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
    //模型颜色
    vec4 color = texture(tex, fTexCoord);
    vec4 color1 = texture(tex1, fTexCoord);
    vec4 modelColor = (color * color.a + color1 * color1.a) / 2;
    
    Material material;
    material.ambient = material.diffuse = modelColor.rgb;
    material.specular = vec3(1.0);
    material.shininess = 256;
    
    vec3 final = calcLight(material, lightDirect);
    final += calcLight(material, lightSpot);
    
    fragColor = vec4(final, 1.0f);
}
