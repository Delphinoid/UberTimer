#include "gfxProgram.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "vertex3D.hpp"

gfxProgram::~gfxProgram(){

	IMG_Quit();
	Mix_Quit();

	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	glDeleteProgram(shaderProgramID);

	if(spriteVaoID != 0){
		glDeleteVertexArrays(1, &spriteVaoID);
	}
	if(spriteVboID != 0){
		glDeleteBuffers(1, &spriteVboID);
	}

}

bool gfxProgram::initPrg(unsigned int windowX, unsigned int windowY, unsigned int windowW, unsigned int windowH, std::string vertexShaderPath, std::string fragmentShaderPath){

	return (initSDL(windowX, windowY, windowW, windowH) && initOGL() && loadShaders(vertexShaderPath, fragmentShaderPath) && createBuffers());

}

bool gfxProgram::initSDL(unsigned int windowX, unsigned int windowY, unsigned int windowW, unsigned int windowH){

	/* Initialize SDL */
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
		printf("Error initializing SDL library: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_VERSION_MAJOR);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_VERSION_MINOR);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create the window and context
	window = SDL_CreateWindow("Uber Timer", windowX, windowY, windowW, windowH, SDL_WINDOW_OPENGL);
	SDL_SetWindowBordered(window, SDL_FALSE);
	context = SDL_GL_CreateContext(window);

	// Disable VSync
	SDL_GL_SetSwapInterval(0);

	// Initialize SDL extension libraries SDL_image and SDL_mixer
	if(!IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG)){
		printf("Error initializing SDL extension library SDL_image: %s\n", IMG_GetError());
		return false;
	}
	if(Mix_OpenAudio(DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, DEFAULT_CHANNELS, DEFAULT_CHUNKSIZE) < 0){
		printf("Error initializing SDL extension library SDL_mixer: %s\n", Mix_GetError());
		return false;
	}

	return true;

}

bool gfxProgram::initOGL(){

	/* Initialize GLEW */
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Error initializing GLEW: %s\n", glewGetErrorString(glewError));
		return false;
	}
	glGetError();  // Flush the error buffer. glewInit() sets it to 1280 (invalid context) with GL versions 3.2 and up

	std::stringstream glVersion;
	glVersion << "GL_VERSION_" << GL_VERSION_MAJOR << "_" << GL_VERSION_MINOR;  // Get GL version string
	if(!glewIsSupported(glVersion.str().c_str())){
		printf("OpenGL v%i.%i is unavailable.\n", GL_VERSION_MAJOR, GL_VERSION_MINOR);
		return false;
	}


	/* Initialize OpenGL */
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error initializing OpenGL: %u\n", glError);
		return false;
	}


	return true;

}

bool gfxProgram::loadShaders(std::string vertexShaderPath, std::string fragmentShaderPath){

	GLuint vertexShaderID;
	std::string vertexShaderCode;
	GLuint fragmentShaderID;
	std::string fragmentShaderCode;


	/* Load vertex shader */
	std::ifstream vertexShaderFile(vertexShaderPath.c_str());
	std::string line;

	if(vertexShaderFile.is_open()){
		while(vertexShaderFile.good()){

			getline(vertexShaderFile, line);
			vertexShaderCode += "\n" + line;

		}
		vertexShaderFile.close();
	}else{
		printf("Error loading vertex shader:\nCouldn't open %s", vertexShaderPath.c_str());
		return false;
	}

	/* Load fragment shader */
	std::ifstream fragmentShaderFile(fragmentShaderPath.c_str());

	if(fragmentShaderFile.is_open()){
		while(fragmentShaderFile.good()){

			getline(fragmentShaderFile, line);
			fragmentShaderCode += "\n" + line;

		}
		fragmentShaderFile.close();
	}else{
		printf("Error loading fragment shader:\nCouldn't open %s", fragmentShaderPath.c_str());
		return false;
	}


	GLint compileStatus = GL_FALSE;
 	int infoLogLength;


	/* Compile vertex shader */
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char *vertexShaderCodePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexShaderCodePointer, NULL);
	glCompileShader(vertexShaderID);

	/* Validate vertex shader */
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 0){
		std::vector<char> vertexShaderError(infoLogLength + 1);
 		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderError[0]);
 		printf(&vertexShaderError[0]);
 		return false;
 	}


	/* Compile fragment shader */
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragmentShaderCodePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentShaderCodePointer, NULL);
	glCompileShader(fragmentShaderID);

	/* Validate fragment shader */
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compileStatus);
 	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
 	if(infoLogLength > 0){
		std::vector<char> fragmentShaderError(infoLogLength + 1);
 		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderError[0]);
 		printf(&fragmentShaderError[0]);
 		return false;
 	}


	/* Link the program */
	shaderProgramID = glCreateProgram();
 	glAttachShader(shaderProgramID, vertexShaderID);
 	glAttachShader(shaderProgramID, fragmentShaderID);
 	glLinkProgram(shaderProgramID);

 	glDetachShader(shaderProgramID, vertexShaderID);
 	glDetachShader(shaderProgramID, fragmentShaderID);
 	glDeleteShader(vertexShaderID);
 	glDeleteShader(fragmentShaderID);

	/* Link the uniform variables */
	mvpMatrixID        = glGetUniformLocation(shaderProgramID, "modelViewProjectionMatrix");
	uberPercentID      = glGetUniformLocation(shaderProgramID, "uberPercent");


	/* Use the program */
	glUseProgram(shaderProgramID);
 	return true;

}

bool gfxProgram::createBuffers(){

	/* Set lastTexID to 0 since we haven't rendered anything yet */
	lastTexID = 0;

	/* VAO and VBO for rendering sprites */
	// Create and bind the sprite VAO
	glGenVertexArrays(1, &spriteVaoID);
	glBindVertexArray(spriteVaoID);
	// Create and bind the sprite VBO
	glGenBuffers(1, &spriteVboID);
	glBindBuffer(GL_ARRAY_BUFFER, spriteVboID);
	// Position offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, pos));
	glEnableVertexAttribArray(0);
	// UV offset
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, u));
	glEnableVertexAttribArray(1);
	// Normals offset
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3D), (GLvoid*)offsetof(vertex3D, nx));
	glEnableVertexAttribArray(2);
	// We don't want anything else to modify the VAO
	glBindVertexArray(0);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error creating buffers:\n%u\n", glError);
		return false;
	}
	return true;

}
