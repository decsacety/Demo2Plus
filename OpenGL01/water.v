#version 460 core//版本号
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

out vec3 Normal;
out vec3 Position;//顶点世界坐标
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 model;

void main()
{
    gl_Position = transform * vec4(aPos.x,aPos.y,aPos.z, 1.0);
    Normal = mat3(transpose(inverse(model))) * aNormal;//对法线向量进行世界坐标的转换
    Position=vec3(model*vec4(aPos,1.0));
    TexCoord=aTex;
}