#version 460 core//�汾��

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;//������������

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;//��Դ����λ��
uniform vec3 viewPos;//�۲���/�����λ��

uniform float offset=0.5;

void main()
{   
    float ambientStrength=0.1;//���������
    vec3 objectColor = texture(texture_diffuse1, TexCoord).rgb;
    
    vec3 lightColor=vec3(0.953f,0.8823f,0.8125f);//�ƹ���ɫ
    vec3 ambient = ambientStrength * objectColor;
    
    //��������ռ���
    vec3 norm = normalize(Normal);//��׼��
    vec3 lightDir= normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse= diff * lightColor;

    //����߹����
    float specularStrength = 0.8;//�趨����߹�ǿ��
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow( max(dot(viewDir , reflectDir),0.0f), 256);// ��ֵ��ʾ�����
    vec3 specular = specularStrength * spec *lightColor;

    vec3 Light= (ambient + diffuse+ specular ) * lightColor;
    //FragColor = vec4(Light,1.0f);
    //FragColor =mix(texture(texture_diffuse1, TexCoord), texture(texture_normal1, TexCoord),offset)*vec4(Light,1.0f);
    FragColor = texture(texture_diffuse1, TexCoord) * vec4(Light,1.0f);
}



/*
#version 460 core//�汾��

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