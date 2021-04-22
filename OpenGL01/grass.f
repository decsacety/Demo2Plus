#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D grass;

void main()
{             
    vec4 texColor = texture(grass, TexCoord);
    if(texColor.a < 0.2)
        discard;
    FragColor = texColor;
    //FragColor = texture(grass, TexCoord);
}