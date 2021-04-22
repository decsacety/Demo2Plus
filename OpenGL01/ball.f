#version 460

in vec2 Tex;
in vec3 Pos;
in vec3 Norm;

#define PI 3.14159265

out vec4 fragcolor;

uniform sampler2D Envir;
uniform vec3 lightPos;

float D_GGX_TR(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H),0);
    float NdotH2 = NdotH*NdotH;

    float denom  = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom        = PI * denom * denom;

    return a2 / denom;
}

void main(){
	//计算光照量
    vec3 lightDir = normalize(lightPos - Pos);

	float a = 0.70;//粗糙度

	float tmp = dot(Norm, lightDir);
	float diff = D_GGX_TR(Norm, lightDir, a);

	vec3 col = texture(Envir, Tex).rgb;
	fragcolor = vec4(col, 1.0f);
}



/*
#version 460

in vec2 Tex;
in vec3 Pos;
in vec3 Norm;

out vec4 fragcolor;

uniform sampler2D Envir;
uniform vec3 lightPos;

void main(){
	//计算光照量
    vec3 lightDir = normalize(lightPos - Pos);
	float diff =  dot(lightDir, Norm);
	if(diff<-0.222){
		diff = (diff+0.222)*(diff+0.222);
	}
	else {
		if(diff<0.778f)
			diff = (cos(diff*3.14+3.14+0.7)+1)/2;
		else 
			diff = (diff-0.778)*(diff - 0.778)+1;
	}


	vec3 col = texture(Envir, Tex).rgb;
	fragcolor = vec4(col*diff*0.8+col*0.2, 1.0f);
   // fragcolor = vec4(0.5f,0.28f,0.4f,1.0f);
}
*/
