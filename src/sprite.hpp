#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <vector>
#include "gfxProgram.hpp"
#include "vertex3D.hpp"
#include "textureWrapper.hpp"

struct sprite{

	vertex3D vertices[4];
	glm::vec3 position;   // Position of the sprite
	float width, height;  // Size of the sprite
	glm::vec3 relPivot;   // The point the sprite is rotated around (relative to its position)
	glm::vec3 rotation;   // Rotation of the sprite in degrees
	glm::vec3 scale;      // Scale of the sprite (z = 1)
	float translucency;   // A value to multiply against the alpha values of the pixels

	bool billboardX;  // Whether or not the sprite uses the camera's rotated X axis
	bool billboardY;  // Whether or not the sprite uses the camera's rotated Y axis
	bool billboardZ;  // Whether or not the sprite uses the camera's rotated Z axis
	bool hudElement;  // Whether or not the sprite is part of the HUD

	textureWrapper *sprTexture;  // Pointer to the texture wrapper being used. Could be pretty dangerous, might need to change it
	unsigned int currentAnim;    // Position of the current animation in the texture wrapper's animations vector
	unsigned int currentFrame;   // The current frame of the animation
	float frameProgress;  // When this is greater than 1, it progresses the animation by frameProgress (rounded towards 0) frames
	                      // and subtracts frameProgress (rounded towards 0) from itself. The lower the FPS, the higher the value
	                      // that will be added to frameProgress and vice versa

	sprite();
	//~sprite();

	bool sprLoad(std::string filePath, std::vector<textureWrapper> &allTextures);
	void sprAnimateTex();
	void sprCreate(gfxProgram &gfxPrg);
	void sprOffsetTexture(float texFrag[4], float texWidth, float texHeight);

};

#endif
