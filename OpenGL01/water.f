#version 460 core//版本号

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoord;

uniform vec3 cameraPos;
uniform samplerCube skybox0;
uniform samplerCube skybox1;
uniform samplerCube skybox2;
uniform samplerCube skybox3;
uniform sampler2D wave;

uniform float offset;
uniform float trpRate=0.3;
uniform bool IsWave=false;

vec4 getTex(){

    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
	float trp=trpRate;

	if(offset>3){
		vec4 tmp=mix(texture(skybox0, R), texture(skybox2, R),offset-3);
		tmp.a=trp;
		return tmp;
	}
		if(offset>2){
		vec4 tmp=mix(texture(skybox3, R), texture(skybox0, R),offset-2);
		tmp.a=trp;
		return tmp;
	}
	if(offset>1){
		vec4 tmp=mix(texture(skybox1, R), texture(skybox3, R),offset-1);
		tmp.a=trp;
		return tmp;
	}
	vec4 tmp=mix(texture(skybox2, R), texture(skybox1, R),offset);
		tmp.a=trp;
		return tmp;
}

void main()
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
	float trp=trpRate;

	if( IsWave ){
		vec4 tmp=getTex();
		FragColor = mix(texture(wave, TexCoord),tmp,0.3);
		return;
	}

    //FragColor = vec4(texture(skybox0, R).rgb, 1.0);
    	//顺序：2-0-3-1
	if(offset>3){
		vec4 tmp=mix(texture(skybox0, R), texture(skybox2, R),offset-3);
		tmp.a=trp;
		FragColor =tmp;//offset为1说明启用了后一个
		return;
	}
	if(offset>2){
		vec4 tmp=mix(texture(skybox3, R), texture(skybox0, R),offset-2);
		tmp.a=trp;
		FragColor =tmp;//offset为1说明启用了后一个
		return;
	}
	if(offset>1){
		vec4 tmp=mix(texture(skybox1, R), texture(skybox3, R),offset-1);
		tmp.a=trp;
		FragColor =tmp;//offset为1说明启用了后一个
		return;
	}
	vec4 tmp=mix(texture(skybox2, R), texture(skybox1, R),offset);
		tmp.a=trp;
		FragColor =tmp;//offset为1说明启用了后一个


    //FragColor=texture(skybox, TexCoord);
    //vec4(0.9f,0.8f,0.8f,1.0f);
    //FragColor =mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord),offset);
}
