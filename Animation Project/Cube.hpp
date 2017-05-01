#ifndef CUBE_HPP
#define CUBE_HPP

#include "Inclusions.h"

const int STEPS_BETWEEN_KEYS = 1000;

const GLfloat cubeverts[24] ={
	-0.5f, -0.5f, 0.5f, 
	0.5f, -0.5f, 0.5f, 
	0.5f, 0.5f, 0.5f, 
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f, 
	0.5f, -0.5f, -0.5f, 
	0.5f, 0.5f, -0.5f, 
	-0.5f, 0.5f, -0.5f
};

const GLfloat cubecols[24] ={
	0.f, 0.f, 1.f,
	1.f, 0.f, 1.f,
	1.f, 1.f, 1.f,
	0.f, 1.f, 1.f,
	0.f, 0.f, 0.f,
	1.f, 0.f, 0.f,
	1.f, 1.f, 0.f,
	0.f, 1.f, 0.f
};

const GLuint cubeinds[36] ={
	4, 5, 6, 4, 6, 7, // BACK
	0, 1, 2, 0, 2, 3, // FRONT
	0, 4, 7, 0, 7, 3, // LEFT
	1, 5, 6, 1, 6, 2, // RIGHT
	0, 1, 5, 0, 5, 4, // BOTTOM
	3, 2, 6, 3, 6, 7  // TOP
};

struct QCube{
	GLuint CVAO, CVBO, CCBO, CEBO;
	mat4 model;
	int id;
	bool isKey;
	GLuint ID, QTN;
	vec4 quaternion;
	int keyA, keyB, time;
	vec4 aKey;
	vec4 bKey;

	QCube(int num);

	~QCube(){
		deleteCube();
	}

	void update();

	void initCube();

	void deleteCube();

	void drawCube();

	void rotate(vec4);

	vec4 slerp(float, vec4, vec4);

	vec4 catmullRom(float, vec4, vec4, vec4, vec4);
};

#endif