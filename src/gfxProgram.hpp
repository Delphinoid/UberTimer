#ifndef GFXPROGRAM_HPP
#define GFXPROGRAM_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 5

#define DEFAULT_WIDTH 104
#define DEFAULT_HEIGHT 70
#define DEFAULT_FREQUENCY 22050
#define DEFAULT_CHANNELS 2
#define DEFAULT_CHUNKSIZE 2048

struct gfxProgram{

	// OpenGL / SDL window and context
	SDL_Window *window;
	SDL_GLContext context;

	// Main uniform variables for the shaders
	GLuint shaderProgramID;
	GLuint mvpMatrixID;
	GLuint uberPercentID;

	// Various matrices for rendering
	glm::mat4 projectionMatrix;

	// Previously bound texture ID for more efficient binding
	GLuint lastTexID;

	// Other uniforms
	GLuint uberPercent;
	GLuint kritzPercent;

	// VAO and VBO for rendering sprites
	GLuint spriteVaoID;
	GLuint spriteVboID;


	~gfxProgram();

	bool initPrg(unsigned int windowX, unsigned int windowY, unsigned int windowW, unsigned int windowH, std::string vertexShaderPath, std::string fragmentShaderPath);
	bool initSDL(unsigned int windowX, unsigned int windowY, unsigned int windowW, unsigned int windowH);
	bool initOGL();
	bool loadShaders(std::string vertexShaderPath, std::string fragmentShaderPath);
	bool createBuffers();

};

#endif
