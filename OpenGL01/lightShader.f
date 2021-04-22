#version 460 core//°æ±¾ºÅ

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform float offset=0.5;

void main()
{
    FragColor=vec4(1.0f,1.0f,1.0f,1.0f);
    //FragColor =mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord),offset);
}
