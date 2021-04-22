#version 460 core//版本号

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;//顶点世界坐标

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform vec3 lightPos;//光源世界位置
uniform vec3 objectColor;//物体本身颜色
uniform vec3 viewPos;//观察者/摄像机位置

uniform float offset=0.5;

void main()
{   
    float ambientStrength=0.1;//环境光分量
    
    vec3 lightColor=vec3(0.90f,1.0f,1.0f);
    vec3 ambient = ambientStrength * lightColor;
    
    //漫反射光照计算
    vec3 norm = normalize(Normal);//标准化
    vec3 lightDir= normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse= diff * lightColor;

    //镜面高光计算
    float specularStrength = 0.8;//设定镜面高光强度
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow( max(dot(viewDir , reflectDir),0.0f), 256);// 幂值表示反光度
    vec3 specular = specularStrength * spec *lightColor;

    vec3 Light= (ambient + diffuse+ specular ) * objectColor;
    //FragColor = vec4(Light,1.0f);
    FragColor =mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord),offset)*vec4(Light,1.0f);
}


/*
#version 460 core//版本号

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform float offset=0.5;

void main()
{
    FragColor =mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord),offset);
}
*/