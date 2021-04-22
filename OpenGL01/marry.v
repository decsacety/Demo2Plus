#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 Tex;
out vec3 Pos;
out vec3 Norm;

uniform mat4 transform;
uniform mat4 model;

void main(){
	gl_Position = model * vec4(aPos, 1.0);
	Norm = mat3(model) * aNorm;//转换到世界坐标
	Pos = vec3(gl_Position);//转换到世界坐标
	gl_Position = transform *gl_Position;
	Tex = aTex;
}
