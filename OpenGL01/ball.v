#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 Tex;
out vec3 Pos;
out vec3 Norm;

uniform mat4 transform;

void main(){
	gl_Position = aInstanceMatrix * vec4(aPos, 1.0);
	Pos = vec3(gl_Position);//转换到世界坐标
	gl_Position = transform *gl_Position;
	Norm = normalize(aNorm);
	Tex = aTex;
}

//gl_Position = transform * aInstanceMatrix * vec4(aPos, 1.0);
//Pos = vec3( aInstanceMatrix * vec4(aPos, 1.0) );

/*
#version 460
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out vec2 Tex;
out vec3 Position;
out vec3 Norm;

uniform mat4 transform;
uniform mat4 model;

void main(){
	gl_Position = transform * vec4(aPos, 1.0);
	Position = vec3(model * vec4(aPos, 1.0));
	Tex = aTex;
	Norm = aNorm;
}
*/