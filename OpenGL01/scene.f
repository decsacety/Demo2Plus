#version 460

in vec2 Tex;

out vec4 fragcolor;

uniform sampler2D Floor;

void main(){
    fragcolor = vec4(0.5f,0.5f,0.7f,1.0f);
    //��ͼ֮���÷���������һ��
}