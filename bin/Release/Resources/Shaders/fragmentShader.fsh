#version 450 core

in vec2 UV;
uniform sampler2D textureSampler;
uniform uint uberPercent;
out vec4 colour;

void main(){
	vec4 tempColour = texture(textureSampler, UV).rgba;
	if(tempColour.a == 0.0){
        discard;
    }
	if(uberPercent >= 100){
		tempColour.r = 0.0;
		tempColour.b = 0.0;
	}else{
		float fUberPercent = uberPercent;
		tempColour.r = (100 - fUberPercent) * 0.01;
		tempColour.g = fUberPercent * 0.01;
		tempColour.b = 0.0;
	}
	colour = tempColour;
}