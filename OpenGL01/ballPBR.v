#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 aInstanceMatrix;//TBN矩阵，用来进行法线贴图的空间转换

out vec2 Tex;
out vec3 Pos;
out vec3 Norm;

uniform mat4 transform;

void main(){
	gl_Position = aInstanceMatrix * vec4(aPos, 1.0);
	Norm = mat3(aInstanceMatrix) * aNorm;//转换到世界坐标
	Pos = vec3(gl_Position);//转换到世界坐标
	gl_Position = transform *gl_Position;
	Tex = aTex;
}
