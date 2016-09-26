#include "texture.hpp"
#include <SDL2/SDL_image.h>
#include <stdio.h>

texture::texture(){
	textureID = 0;
	textureWidth = 0;
	textureHeight = 0;
	translucent = false;
}

texture::texture(const texture &copiedTex){

	textureID = copiedTex.textureID;
	textureWidth = copiedTex.textureWidth;
	textureHeight = copiedTex.textureHeight;
	translucent = copiedTex.translucent;

	copiedTex.textureID = 0;  // Sets the old texture's textureID to 0 so its destructor won't call glDeleteTextures()

}

texture::~texture(){
	if(textureID != 0){
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
}

bool texture::createTexture(const char *texPath){

	/* Load image with SDL_Image */
	SDL_Surface *SDLimage = IMG_Load(texPath);
	if(SDLimage == NULL){
		printf("Error generating SDL_Surface for texture at %s:\n%s\n", texPath, SDL_GetError());
		return false;
	}

	GLint pixelFormat;
	if(SDLimage->format->BytesPerPixel == 3){
		pixelFormat = GL_RGB;  // 24 bits per pixel is most likely the RGB format
	}else if(SDLimage->format->BytesPerPixel == 4){
		pixelFormat = GL_RGBA; // 32 bits per pixel is most likely the RGBA format
	}

	textureWidth = SDLimage->w;
	textureHeight = SDLimage->h;


	/* Convert it to an OpenGL texture and free the SDL Surface */
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, SDLimage->w, SDLimage->h, 0, pixelFormat, GL_UNSIGNED_BYTE, SDLimage->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum glError = glGetError();
	if(glError != GL_NO_ERROR){
		printf("Error generating OpenGL texture at %s:\n%u\n", texPath, glError);
		return false;
	}


	/* Check if the texture contains translucent (not just transparent) pixels and then free the SDL surface */
	if(pixelFormat == GL_RGBA){

		uint8_t *pixelData = (uint8_t*)SDLimage->pixels;

		for(uint32_t d = 0; d < textureWidth * textureHeight; d++){

			uint8_t alpha = pixelData[d*4+3];

			if(alpha > 0 && alpha < 255){
				translucent = true;
				d = textureWidth * textureHeight;
			}

		}

	}
	SDL_FreeSurface(SDLimage);


	return true;

}
