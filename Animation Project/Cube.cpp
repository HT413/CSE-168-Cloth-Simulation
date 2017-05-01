#include "Cube.hpp"
#include "Window.hpp"

// Ctor func
QCube::QCube(int num){
	isKey = true;
	id = num;
	model = mat4(1.f);
	initCube();
	QTN = glGetUniformLocation(basicShader, "quat");
	quaternion = vec4(1.f, 0.f, 0.f, 0.f);
	keyA = 1; keyB = 2; time = 0;
}

// Idle update func
void QCube::update(){
	if(isKey) return; // Do nothing if it's a key
	// Otherwise update this cube
	++time;
	if(time >= STEPS_BETWEEN_KEYS){
		time -= STEPS_BETWEEN_KEYS;
		keyA++;
		keyB++;
		if(keyB == cubes.size()){
			keyA = 1;
			keyB = 2;
		}
		aKey = cubes[keyA]->quaternion;
		bKey = cubes[keyB]->quaternion;
	}
	float t = (float)time / (float)STEPS_BETWEEN_KEYS;
	if(useCR){
		for(int i = 2; i < cubes.size(); i++){
			if(glm::dot(cubes[i-1]->quaternion, cubes[i]->quaternion) < 0){
				cubes[i]->quaternion = -(cubes[i]->quaternion);
			}
		}
		if(keyA == 1){
			quaternion = catmullRom(t, cubes[keyA]->quaternion, cubes[keyA]->quaternion, cubes[keyB]->quaternion, cubes[keyB + 1]->quaternion);
		}
		else if(keyA == cubes.size() - 2){
			quaternion = catmullRom(t, cubes[keyA - 1]->quaternion, cubes[keyA]->quaternion, cubes[keyB]->quaternion, cubes[keyB]->quaternion);
		}
		else{
			quaternion = catmullRom(t, cubes[keyA - 1]->quaternion, cubes[keyA]->quaternion, cubes[keyB]->quaternion, cubes[keyB + 1]->quaternion);
		}
	}
	else{
		if(glm::dot(cubes[keyA]->quaternion, cubes[keyB]->quaternion) < 0)
			quaternion = slerp(t, -aKey, bKey);
		else
			quaternion = slerp(t, aKey, bKey);
	}
	model = (1 - t) * cubes[keyA]->model + t * cubes[keyB]->model;
}

// Init func
void QCube::initCube(){
	glGenVertexArrays(1, &CVAO);
	glGenBuffers(1, &CVBO);
	glGenBuffers(1, &CCBO);
	glGenBuffers(1, &CEBO);
	// Bind the cube geometry data
	glBindVertexArray(CVAO);
	// Geometry data
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, CVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeverts), cubeverts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Color data
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, CCBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubecols), cubecols, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	// Index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeinds), cubeinds, GL_STATIC_DRAW);
	// Done binding data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Dtor func
void QCube::deleteCube(){
	glDeleteVertexArrays(1, &CVAO);
	glDeleteBuffers(1, &CVBO);
	glDeleteBuffers(1, &CCBO);
	glDeleteBuffers(1, &CEBO);
}

// Draw func to render the cube
void QCube::drawCube(){
	if(isKey){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Don't fill polygons
		glUniform1i(uniformKey, 1);
	}
	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill polygons
		glUniform1i(uniformKey, 0);
	}
	glUniform4fv(QTN, 1, &(quaternion[0]));
	glUniformMatrix4fv(uniformM, 1, GL_FALSE, &(model[0][0]));
	glBindVertexArray(CVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// Rotate func. Rotates the key.
void QCube::rotate(vec4 other){
	quaternion = vec4(quaternion.x * other.x - quaternion.y * other.y - quaternion.z * other.z - quaternion.w * other.w,
		              quaternion.x * other.y + quaternion.y * other.x - quaternion.z * other.w + quaternion.w * other.z,
		              quaternion.x * other.z + quaternion.z * other.x + quaternion.y * other.w - quaternion.w * other.y,
		              quaternion.x * other.w + quaternion.w * other.x - quaternion.y * other.z + quaternion.z * other.y);
	quaternion = glm::normalize(quaternion); // Just in case of float point roundoff errors
}

// Spherical interpolation helper func
vec4 QCube::slerp(float t, vec4 a, vec4 b){
	float angle = acosf(glm::dot(a, b));
	return (angle >= -0.001f && angle <= 0.001f)? a : sinf((1 - t) * angle) * a / sinf(angle) + sinf(t*angle) * b / sinf(angle);
}

// Catmull-Rom interpolation helper func
vec4 QCube::catmullRom(float t, vec4 q0, vec4 q1, vec4 q2, vec4 q3){
	vec4 q10 = slerp(t + 1.f, q0, q1);
	vec4 q11 = slerp(t, q1, q2);
	vec4 q12 = slerp(t - 1.f, q2, q3);
	vec4 q20 = slerp((t + 1.f)/2.f, q10, q11);
	vec4 q21 = slerp(t / 2.f, q11, q12);
	return slerp(t, q20, q21);
}