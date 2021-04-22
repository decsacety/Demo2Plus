#version 460 core

in vec2 TexCoord;
in vec3 Pos;
in vec3 Norm;
in mat3 TBN;

out vec4 fragColor;

uniform sampler2D head;
uniform sampler2D normal1;
uniform sampler2D normal2;
uniform vec3 lightPos;

//��������
uniform float offset;

void main(){
	//����Ԥ����
	vec3 Normal1 =texture(normal1, TexCoord).rgb;
	vec3 Normal2 =texture(normal2, TexCoord).rgb;
	Normal1 = Normal1*2.0f - 1.0f;
	Normal2 = Normal2*2.0f - 1.0f;
	Normal1 = TBN * Normal1;//ͨ��TBN����������߿ռ�ת��������ռ�����
	Normal2 = TBN * Normal2;

	//���������

	vec3 norm = normalize(Norm);
    vec3 lightDir = normalize(lightPos - Pos);
	float diff1 = max(dot(lightDir, Normal1), 0);
	float diff2 = max(dot(lightDir, Normal2), 0);
	float diff = diff1*offset + diff2*(1 - offset);

	fragColor = vec4(diff*texture(head, TexCoord).rgb,1.0f);//*texture(head, TexCoord);
	//fragColor = texture(head, TexCoord);
	//fragColor = vec4(T,1.0f);
}
