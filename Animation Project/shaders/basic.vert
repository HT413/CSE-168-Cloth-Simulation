// A basic vertex shader.
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 Color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; // Treat as self translation. Cubes will be stretched
// Other uniforms
uniform mat4 stretch;
uniform vec4 quat;

// In case derivation of a rotation matrix is needed
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

mat4 quaternionToEuler(vec4 q){
	return mat4(1 - 2*q.z*q.z - 2*q.w*q.w, 2*q.y*q.z + 2*q.x*q.w,     2*q.y*q.w - 2*q.x*q.z,     0.0,
	            2*q.y*q.z - 2*q.x*q.w,     1 - 2*q.y*q.y - 2*q.w*q.w, 2*q.z*q.w + 2*q.x*q.y,     0.0,
				2*q.y*q.w + 2*q.x*q.z,     2*q.z*q.w - 2*q.x*q.y,     1 - 2*q.y*q.y - 2*q.z*q.z, 0.0,
				0.0,                       0.0,                       0.0,                       1.0);
}

void main(){
	gl_Position = projection * view * model * quaternionToEuler(quat) * stretch * vec4(position, 1.0f);
	Color = color;
}