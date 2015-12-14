#pragma once
#include "commonHeader.h"
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include "SOIL.h"
class OpenGlInit {
private:
	GLFWwindow* window;
	GLuint programID;
	GLuint MatrixID=-1;
	GLuint Texture;
	GLuint TextureID;
	GLuint VertexArrayID;
	GLuint vertexbuffer;
	GLuint uvbuffer;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	GLfloat *arrayn = NULL;
public:
	int Init3D(int modeCode) {
		if (!glfwInit())
		{
			fprintf(stderr, "Failed to initialize GLFW\n");
			getchar();
			return -1;
		}

		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(win_w_c, win_h_c, "Tutorial 07 - Model Loading", NULL, NULL);
		if (window == NULL) {
			fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
			getchar();
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glewExperimental = true; // Needed for core profile
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW\n");
			getchar();
			glfwTerminate();
			return -1;
		}
		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		
		//setup the matrix
		
		ProjectionMatrix = glm::mat4(1.0f);
		ViewMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::mat4(1.0f);
		if (modeCode == 1) {
			buildTri();
		}
		else if (modeCode == 2) {
			buildBall();
		}
		
		MatrixID = glGetUniformLocation(programID, "MVP");
		arrayn = new GLfloat[3 * win_w_c*win_h_c];

	}

	void moveCam(float x, float y, float z) {
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(x, y, z));
	}

	void setCam(float x, float y, float z) {
		//ViewMatrix = glm::mat4(1.0f);
		//ViewMatrix[3][0] = -x;
		//ViewMatrix[3][1] = -y;
		//ViewMatrix[3][2] = 3;
		ViewMatrix = glm::lookAt(
			glm::vec3(x, y, z), // Camera is at (4,3,3), in World Space
			glm::vec3(x, y, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
	}

	void renderOneFrame() {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glUniform1i(TextureID, 0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		//glDisableVertexAttribArray(0);
		//glDisableVertexAttribArray(1);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	float* clipScreen() {
		try {
			glReadnPixels(0, 0, win_w_c, win_h_c, GL_RGB, GL_FLOAT, win_w_c*win_h_c * 3 * sizeof(GLfloat), arrayn);
		}
		catch (int e) {
			if (e == GL_INVALID_OPERATION) {
				printf("read img wrong");
				return NULL;
			}
		}
		return arrayn;
	}


	void cleanMem() {
		glDeleteBuffers(1, &vertexbuffer);
		glDeleteBuffers(1, &uvbuffer);
		glDeleteProgram(programID);
		glDeleteTextures(1, &TextureID);
		glDeleteVertexArrays(1, &VertexArrayID);
		delete[] arrayn;
	}

	void closeWindow() {
		glfwTerminate();
	}
private:
	void buildBall() {
		programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
		int width, height;
		unsigned char* image = SOIL_load_image("loli.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		//glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		try {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		}
		catch (int e) {
			int a = 1;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SOIL_free_image_data(image);
		//Texture = loadDDS("uvmap.DDS");
		//TextureID = glGetUniformLocation(programID, "myTextureSampler");

		//setup the vertice
		bool res = loadOBJ("sphere.obj", vertices, uvs, normals);
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 10.0f);
		//ProjectionMatrix[2][3] = ProjectionMatrix[2][3] * -1;
		//ProjectionMatrix[3][2] = ProjectionMatrix[3][2] * -1;
		glUseProgram(programID);
	}

	void buildTri() {
		programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
		vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
		vertices.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
		vertices.push_back(glm::vec3(0.0f, 0.5f, 0.0f));
		
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		glUseProgram(programID);
	}
};

