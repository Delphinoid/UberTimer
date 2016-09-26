#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL_opengl.h>

struct texture{

	mutable GLuint textureID;  // Mutable so glDeleteTextures() isn't called when pushing a texture into a vector
	GLuint textureWidth;
	GLuint textureHeight;
	bool translucent;

	texture();
	texture(const texture &copiedTex);  // Copy constructor for use with std::vector
	~texture();

	bool createTexture(const char *texPath);
	void checkTranslucency();

};

#endif
