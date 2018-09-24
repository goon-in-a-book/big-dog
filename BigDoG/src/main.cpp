#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm> 
#include <stdlib.h>
#include <time.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Program.h"

int main(void)
{
	// Sorting speed test
	/*
	srand(time(NULL));
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::vector<float> bigArray(1024 * 1024, 0);
	for (size_t i = 0; i < bigArray.size(); i++) {
		bigArray[i] = rand() % 1000000 + 1;
	}
	std::sort(bigArray.begin(), bigArray.end());
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Time elapsed = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
	std::cin.get();*/

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	GLFWwindow* window;

	if (!glfwInit())
		return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a windowed mode window and its OpenGL context
	unsigned int width = 4096, height = 4096;
	window = glfwCreateWindow(width, height, "BigDoG", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	std::cout << "OpenGL version supported by this platform: " << glGetString(GL_VERSION) << std::endl;

	int ok = glewInit();
	if (ok != GLEW_OK) {
		std::cerr << "Error while initializing glew" << std::endl;
		return 1;
	}
	//--------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------

	VertexArray va;

	float positions[8] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};
	VertexBuffer vb(positions, 8 * sizeof(float));
	VertexBufferLayout vbLayout;
	vbLayout.Push<float>(2);
	va.AddBuffer(vb, vbLayout);

	float uv_buffer_data[8] = {
		0.0f, 0.f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	VertexBuffer uvb(uv_buffer_data, 8 * sizeof(float));
	VertexBufferLayout uvbLayout;
	uvbLayout.Push<float>(2);
	va.AddBuffer(uvb, uvbLayout);

	unsigned int indices[6] = {
		0, 1, 2,
		0, 2, 3
	};
	IndexBuffer ib(indices, 6);

	std::vector<float> data(width * height * 4, 0);

	Program renderProgram(
		"res/shaders/render_to_texture_vertex.shader",
		"res/shaders/render_to_texture_fragment.shader"
	);
	renderProgram.Bind();

	//-------------------------------------------------------------------------------------------------
	GLuint FramebufferName = 0;
	GLCall(glGenFramebuffers(1, &FramebufferName));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName));

	// The texture we're going to render to
	GLCall(glActiveTexture(GL_TEXTURE2));
	GLuint renderedTexture;
	GLCall(glGenTextures(1, &renderedTexture));

	// "Bind" the newly created texture : all future texture functions will modify this texture
	GLCall(glBindTexture(GL_TEXTURE_2D, renderedTexture));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, nullptr));

	// Poor filtering. Needed!
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

	// Set "renderedTexture" as our colour attachement #0
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0));

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers)); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error in framebuffer status" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName));

	//-------------------------------------------------------------------------------------------------
	const size_t dataSize = width * height * 4;
	std::vector<float> textureData(dataSize, 1.0f);
	for (size_t i = 0; i < height; i++)
	{
		for (size_t j = 0; j < width; j++) {
			float val = (float)j;
			size_t pos = (i * height * 4) + (j * 4);
			textureData[pos] = val;
			textureData[pos + 1] = val;
			textureData[pos + 2] = val;
		}
	}

	std::chrono::steady_clock::time_point done = std::chrono::steady_clock::now();
	std::cout << "Start up time = " << std::chrono::duration_cast<std::chrono::milliseconds>(done - start).count() << std::endl;

	//------------------------------------------------------------------------------------------------
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLuint dataTexture;
	GLCall(glGenTextures(1, &dataTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, dataTexture));
	GLCall(unsigned int dataLocation = glGetUniformLocation(renderProgram.GetRendererID(), "dataTex"));
	GLCall(glUniform1i(dataLocation, 0));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, &textureData[0]));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

	// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	size_t filterWidth = 256;
	size_t filterSize = filterWidth * filterWidth * 4;
	std::vector<float> filterableValues(filterSize, 0.0f);
	size_t numValsPerRow = filterWidth - 20;
	for (size_t i = 0; i < filterWidth; i++)
	{
		float val = (float)numValsPerRow;
		size_t pos = (i * filterWidth * 4);
		filterableValues[pos] = val;
	}
	for (size_t i = 0; i < 1; i++)
	{
		for (size_t j = 0; j < numValsPerRow; j++)
		{
			float val = (float)(rand() % width);
			size_t pos = (i * filterWidth * 4) + (j * 4) + 4;
			filterableValues[pos] = val;
		}
	}

	//-------------------------------------------------------------------------------------------------
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLuint filterValuesTexture;
	GLCall(glGenTextures(1, &filterValuesTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, filterValuesTexture));
	GLCall(unsigned int filterLocation = glGetUniformLocation(renderProgram.GetRendererID(), "filterTex"));
	GLCall(glUniform1i(filterLocation, 1));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, filterWidth, filterWidth, 0, GL_RGBA, GL_FLOAT, &filterableValues[0]));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));

	// ------------------------------------------------------------------------------------------------

	// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	// std::cout << "Elapsed Time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

	// std::cout << dataLocation << " " << filterLocation << std::endl;
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &data[0]));
	auto end = std::chrono::steady_clock::now();
	std::cout << "Elapsed Time (milliseconds) = " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() / 1000000.0 << std::endl;

	// Examine result
	for (size_t i = 0; i < 4; i++) {
		auto timeStart = std::chrono::steady_clock::now();
		GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &data[0]));
		auto timeEnd = std::chrono::steady_clock::now();
		std::cout << "Transfer Time (milliseconds) = " << std::chrono::duration_cast<std::chrono::nanoseconds>(timeEnd - timeStart).count() / 1000000.0 << std::endl;
	}

	for (size_t i = 0; i < 32; i += 4) {
		std::cout <<
			data[i] << ", " <<
			data[i + 1] << ", " <<
			data[i + 2] << ", " <<
			data[i + 3] << std::endl;
	}

	//-------------------------------------------------------------------------------------------------

	/*
	unsigned int textureId;
	GLCall(glGenTextures(1, &textureId));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureId));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

	std::vector<float> textureData(width * height * 4, 1.0f);
	for (size_t i = 0; i < textureData.size(); i += 4)
	{
		textureData[i] = i / ((float)textureData.size());
		textureData[i + 1] = i / ((float)textureData.size());
		textureData[i + 2] = i / ((float)textureData.size());
	}
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &textureData[0]));
	*/
	/*
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, renderedTexture));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	Program program("res/shaders/vertexShader.shader", "res/shaders/fragmentShader.shader");

	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	program.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	glfwSwapBuffers(window);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwTerminate();
	*/
	return 0;
}