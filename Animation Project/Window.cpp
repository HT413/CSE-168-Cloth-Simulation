#include "Window.hpp"
#include "Camera.h"
#include "Shader.hpp"
#include "Cube.hpp"

// Global variables and functions
enum MOUSE_ACTION {M_NONE, C_ROTATE};
MOUSE_ACTION mouse_action;
GLFWwindow* window; // The main graphics window
int window_w, window_h; // Window width and height
mat4 projection; // Projection and view matrices
double lastX, lastY; // Last cursor x and y coords
GLuint uniformP, uniformV, uniformM; // Projection, view, and model matrix locations
GLuint uniformKey; // Determines whether this cube is a key or not
vector<QCube*> cubes; // List of cubes
int selected = 0;
const float ONE_DEG = 2.f/180.f*M_PI; // One degree, in radians
bool useCR = false; // Determine whether or not to use Catmull-Rom interpolation

// Shader programs to be used
GLuint basicShader;

// Other file specific vars
GLuint STR;
const mat4 identityMat(1.f);
const mat4 stretch = glm::scale(mat4(1.f), vec3(1, 2, 1));

void init_window(int w, int h, const char* name){
	// Attempt to initialize GLFW first
	if(!glfwInit()){
		cerr << "Could not initialize GLFW!" << endl;
		PROGERR(51);
	}
	glfwWindowHint(GLFW_SAMPLES, 2); // 2x anti-aliassing
									 // Attempt to make GLFW window
	window = glfwCreateWindow(w, h, name, 0, 0);
	// Check if window creation successful
	if(!window){
		cerr << "Could not create a GLFW window!" << endl;
		glfwTerminate();
		PROGERR(52);
	}
	cout << "Window intiailized successfully!" << endl;
	// Set this window as the OpenGL context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Swap every frame
						 // Get width & height of framebuffer to properly resize it
	glfwGetFramebufferSize(window, &w, &h);
	// Update everything now to make sure we can draw properly
	window_resize(window, w, h);
	setLookAt();
}

// Object initialization func
void init_objects(){
	basicShader = LoadShaders("shaders/basic.vert", "shaders/basic.frag");
	glUseProgram(basicShader);
	mouse_action = M_NONE;

	// Uniform variables
	uniformP = glGetUniformLocation(basicShader, "projection");
	uniformV = glGetUniformLocation(basicShader, "view");
	uniformKey = glGetUniformLocation(basicShader, "isKey");
	uniformM = glGetUniformLocation(basicShader, "model");
	STR = glGetUniformLocation(basicShader, "stretch");
	// Create the cubes
	cubes.push_back(new QCube(0));
	cubes.push_back(new QCube(1));
	cubes.push_back(new QCube(2));
	cubes.push_back(new QCube(3));
	cubes.push_back(new QCube(4));
	cubes.push_back(new QCube(5));
	// Translate these cubes and position them a bit
	// The actual cube we are using for interpolation
	cubes[0]->isKey = false;
	cubes[0]->model = glm::translate(identityMat, vec3(-10, 0, 0));
	// Keys below
	// Key 1: initialize upright
	cubes[1]->model = glm::translate(identityMat, vec3(-10, 0, 0));
	// Key 2: 90 degree rotation about x
	cubes[2]->model = glm::translate(identityMat, vec3(-5, 0, 0));
	cubes[2]->quaternion = normalize(vec4(sqrtf(0.5), sqrtf(0.5), 0, 0));
	// Key 3: 90 degree rotation about z
	cubes[3]->model = glm::translate(identityMat, vec3(0, 0, 0));
	cubes[3]->quaternion = normalize(vec4(sqrtf(0.5), 0, 0, sqrtf(0.5)));
	// Key 4: 45 degree rotation about y
	cubes[4]->model = glm::translate(identityMat, vec3(5, 0, 0));
	cubes[4]->quaternion = normalize(vec4(cosf(M_PI / 8), 0, sinf(M_PI / 8), 0));
	// Key 5: 45 degree rotation about x
	cubes[5]->model = glm::translate(identityMat, vec3(10, 0, 0));
	cubes[5]->quaternion = normalize(vec4(cosf(M_PI / 8), sinf(M_PI / 8), 0, 0));
	// Misc initializations
	cubes[0]->aKey = cubes[1]->quaternion;
	cubes[0]->bKey = cubes[2]->quaternion;
}

// Object deletion func
void clear_objects(){
	glDeleteProgram(basicShader);
	cubes.clear();
}

// Window resize callback
void window_resize(GLFWwindow* window, int w, int h){
	window_w = w;
	window_h = h;
	// Change the viewport accordingly
	glViewport(0, 0, w, h);
	// Change the projection and view matrix accordingly
	//projection = glm::perspective(M_PI/2.f, (float)w / (float)h, 0.1f, 100.f);
	projection = glm::ortho(-13.5f, 13.5f, -4.5f, 4.5f, 0.1f, 100.f);
	setLookAt();
}

// Window idle callback
void window_idle(){
	// Update the cube
	for(QCube* cube: cubes)
		cube->update();
}

// Window display callback
void window_display(GLFWwindow* window){
	// Check for window events
	glfwPollEvents();
	// Clear window
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// Update the uniform values
	glUseProgram(basicShader);
	glUniformMatrix4fv(uniformP, 1, GL_FALSE, &(projection[0][0]));
	glUniformMatrix4fv(uniformV, 1, GL_FALSE, &(view[0][0]));
	glUniformMatrix4fv(STR, 1, GL_FALSE, &(stretch[0][0]));
	// Now draw objects
	for(QCube* cube: cubes)
		cube->drawCube();
	// Swap buffers now that we're done
	glfwSwapBuffers(window);
}

// Window keyboard callback
void window_key(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_PRESS){
		switch(key){
		// Toggle Catmull-Rom interpolation
		case GLFW_KEY_C:
			useCR = !useCR;
			break;

		// Move first key
		case GLFW_KEY_1:
			selected = 1;
			break;

		// Move second key
		case GLFW_KEY_2:
			selected = 2;
			break;

		// Move third key
		case GLFW_KEY_3:
			selected = 3;
			break;

		// Move fourth key
		case GLFW_KEY_4:
			selected = 4;
			break;

		// Move fifth key
		case GLFW_KEY_5:
			selected = 5;
			break;

		// Rotate selected key by -1 degree in the x axis
		case GLFW_KEY_UP:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(-ONE_DEG / 2), sinf(-ONE_DEG / 2), 0, 0));
			break;


		// Rotate selected key by +1 degree in the x axis
		case GLFW_KEY_DOWN:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), sinf(ONE_DEG / 2), 0, 0));
			break;

		// Rotate selected key by -1 degree in the y axis
		case GLFW_KEY_LEFT:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(-ONE_DEG / 2), 0, sinf(-ONE_DEG / 2), 0));
			break;

		// Rotate selected key by 1 degree in the y axis
		case GLFW_KEY_RIGHT:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, sinf(ONE_DEG / 2), 0));
			break;

		// Rotate selected key 1 degree in the z axis
		case GLFW_KEY_N:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, 0, sinf(ONE_DEG / 2)));
			break;

		// Rotate selected key -1 degree in the z axis
		case GLFW_KEY_M:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, 0, -sinf(ONE_DEG / 2)));
			break;

		// Translate the thing up
		case GLFW_KEY_W:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.f, 0.025f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing down
		case GLFW_KEY_S:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.f, -0.025f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing left
		case GLFW_KEY_A:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(-0.01f, 0.f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing right
		case GLFW_KEY_D:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.01f, 0.f, 0.f)) * cubes[selected]->model;
			break;

		// Terminate when Esc pressed
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		// Don't move any keys
		default:
			selected = 0;
			break;
		}
	}
	else if(action == GLFW_REPEAT){
		switch(key){
		// Rotate selected key by -1 degree in the x axis
		case GLFW_KEY_UP:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(-ONE_DEG / 2), sinf(-ONE_DEG / 2), 0, 0));
			break;


		// Rotate selected key by +1 degree in the x axis
		case GLFW_KEY_DOWN:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), sinf(ONE_DEG / 2), 0, 0));
			break;

		// Rotate selected key by -1 degree in the y axis
		case GLFW_KEY_LEFT:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(-ONE_DEG / 2), 0, sinf(-ONE_DEG / 2), 0));
			break;

		// Rotate selected key by 1 degree in the y axis
		case GLFW_KEY_RIGHT:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, sinf(ONE_DEG / 2), 0));
			break;

		// Rotate selected key 1 degree in the z axis
		case GLFW_KEY_N:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, 0, sinf(ONE_DEG / 2)));
			break;

		// Rotate selected key -1 degree in the z axis
		case GLFW_KEY_M:
			if(selected)
				cubes[selected]->rotate(vec4(cosf(ONE_DEG / 2), 0, 0, -sinf(ONE_DEG / 2)));
			break;

		// Translate the thing up
		case GLFW_KEY_W:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.f, 0.025f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing down
		case GLFW_KEY_S:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.f, -0.025f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing left
		case GLFW_KEY_A:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(-0.01f, 0.f, 0.f)) * cubes[selected]->model;
			break;

		// Translate the thing right
		case GLFW_KEY_D:
			if(selected)
				cubes[selected]->model = glm::translate(identityMat, vec3(0.01f, 0.f, 0.f)) * cubes[selected]->model;
			break;
		}
	}
}

// Mouse cursor moved callback
void window_cursor(GLFWwindow* window, double x, double y){
	if(mouse_action == C_ROTATE){
		mat4 rotationX = glm::rotate(m_identity, ((float)(lastX - x)) / 120.f, vec3(0.f, 1.f, 0.f));
		cam_e = vec3(rotationX * vec4(cam_e, 1.f));
		cam_up = vec3(rotationX * vec4(cam_up, 1.f));
		vec3 axis = glm::cross((cam_e - cam_d), cam_up);
		mat4 rotationY = glm::rotate(m_identity, ((float)(y - lastY)) / 120.f, axis);
		cam_e = vec3(rotationY * vec4(cam_e, 1.f));
		cam_up = vec3(rotationY * vec4(cam_up, 1.f));
		setLookAt();
		// Done rotating, update x and y
		lastX = x;
		lastY = y;
	}
}

// Mouse button callback
void window_mouse(GLFWwindow* window, int button, int action, int mods){
	// Mouse released, do nothing
	if(action == GLFW_RELEASE){
		mouse_action = M_NONE;
	}
	// Mouse pressed, determine appropriate action
	else{
		glfwGetCursorPos(window, &lastX, &lastY);
		if(button == GLFW_MOUSE_BUTTON_1){
			mouse_action = C_ROTATE;
		}
	}
}

// Mouse scroll callback
void window_scroll(GLFWwindow* window, double dx, double dy){
	// Determine if zoom or not
	(dy > 0.0)? cam_e *= 0.98f : cam_e *= 1.02f;
	setLookAt();
}