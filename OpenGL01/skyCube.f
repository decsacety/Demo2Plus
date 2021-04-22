#version 460 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox0;
uniform samplerCube skybox1;
uniform samplerCube skybox2;
uniform samplerCube skybox3;
 

uniform float offset=0;

void main(){
	//FragColor = texture(skyboxNoon, TexCoord);

	/*
	vec3 envColor = textureLod(skybox0, TexCoord, offset).rgb;
	FragColor = vec4(envColor, 1.0f);
	return;
	*/

	//顺序：2-0-3-1
	if(offset>3){
	    vec4 tmp=mix(texture(skybox0, TexCoord), texture(skybox2, TexCoord),offset-3);
		FragColor =mix(texture(skybox0, TexCoord), texture(skybox2, TexCoord),offset-3);//offset为1说明启用了后一个
		return;
	}
	if(offset>2){
		FragColor =mix(texture(skybox3, TexCoord), texture(skybox0, TexCoord),offset-2);//offset为1说明启用了后一个
		return;
	}
	if(offset>1){
		FragColor =mix(texture(skybox1, TexCoord), texture(skybox3, TexCoord),offset-1);//offset为1说明启用了后一个
		return;
	}
	FragColor =mix(texture(skybox2, TexCoord), texture(skybox1, TexCoord),offset);
	
}