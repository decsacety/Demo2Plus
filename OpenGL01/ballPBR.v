#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 aInstanceMatrix;//TBN�����������з�����ͼ�Ŀռ�ת��

out vec2 Tex;
out vec3 Pos;
out vec3 Norm;

uniform mat4 transform;

void main(){
	gl_Position = aInstanceMatrix * vec4(aPos, 1.0);
	Norm = mat3(aInstanceMatrix) * aNorm;//ת������������
	Pos = vec3(gl_Position);//ת������������
	gl_Position = transform *gl_Position;
	Tex = aTex;
}
