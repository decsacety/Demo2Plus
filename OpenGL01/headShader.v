#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec2 TexCoord;
out vec3 Pos;
out vec3 Norm;
out mat3 TBN;

uniform mat4 transform;
uniform mat4 model;

void main(){
    //创建TBN矩阵
    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    TBN = mat3(T, B, N);

    Norm = normalize(aNormal);
    Pos  = vec3(model * vec4(aPos, 1.0));//转换到世界坐标
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTex;
}