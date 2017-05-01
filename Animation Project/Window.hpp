#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Inclusions.h"
#include "Cube.hpp"
#include <vector>

// Global variables and functions
extern GLFWwindow* window; // The main graphics window
extern int window_w, window_h; // Window width and height
extern vec3 cam_e, cam_d, cam_up; // Camera vectors
extern mat4 projection, view; // Projection and view matrices
extern GLuint uniformP, uniformV, uniformM; // PVM matrix uniform locations
extern GLuint uniformKey; // Determines whether this is a key or not
extern GLuint basicShader; // The shader program
extern vector<QCube *> cubes; // List of cubes
extern const mat4 identityMat;
extern const float ONE_DEG;
extern bool useCR;

void init_window(int, int, const char*);
void init_objects();
void clear_objects();
void window_resize(GLFWwindow*, int, int);
void window_idle();
void window_display(GLFWwindow*);
void window_key(GLFWwindow*, int, int, int, int);
void window_cursor(GLFWwindow*, double, double);
void window_mouse(GLFWwindow*, int, int, int);
void window_scroll(GLFWwindow*, double, double);

#endif