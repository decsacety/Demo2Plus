#version 460 core//°æ±¾ºÅ;

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
   vec3 col = texture(screenTexture, TexCoord).rgb;
   /*if(col == vec3(0.5f,0.5f,0.7f)){
		FragColor = vec4(col , 0.5f);
		return;
   }*/
   FragColor = vec4(col, 1.0f);
}