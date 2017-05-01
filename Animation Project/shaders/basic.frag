// A basic fragment shader
#version 330 core

// Inputs and outputs
in vec3 Color;
out vec4 fragColor;

// Uniform variables
uniform vec3 camPos; // Camera position
uniform int isKey;

void main(){
	vec4 finalColor;
	if(isKey == 1){
		finalColor = vec4(0.7, 0.7, 0.2, 0.5);
	} else{
		finalColor = vec4(Color, 1.0);
	}
	fragColor = finalColor;
}