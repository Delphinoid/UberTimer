#ifndef TEXTUREWRAPPER_HPP
#define TEXTUREWRAPPER_HPP

#include "texture.hpp"
#include <string>
#include <vector>

class textureWrapper{

	public:
		// Simple rectangle structure for the frame boundaries when using sprite sheets
		struct rectangle{
			unsigned int x, y, w, h;
			void create(unsigned int inX, unsigned int inY, unsigned int inW, unsigned int inH){ x = inX; y = inY; w = inW; h = inH; }
		};

		// Contains details describing a single image
		struct textureFrame{
			texture baseTexture;  // The actual image
			std::vector<rectangle> subframes;  // Frame bounds (always contains one element, or multiple for sprite sheets)
			texture normalTexture;  // Normal map for the image
		};
		std::vector<textureFrame> allFrames;

		// Contains details describing an animation
		struct textureAnim{
			bool loop;  // Whether or not the animation loops
			std::vector<unsigned int> frameIDs;  // The position of the frame in allFrames
			std::vector<unsigned int> subframeIDs;  // The position of the subframes in textureFrame.subframes
			std::vector<float> frameDelays;  // How long each frame should last
		};
		std::vector<textureAnim> allAnimations;


	public:
		std::string textureName;  // Used when loading sprites and models

		bool texLoad(std::string prgPath, std::string texInfoPath);
		void texAnimate(float speedMod, unsigned int &currentAnim, unsigned int &currentFrame, float &frameProgress);
		GLuint texGetWidth(unsigned int currentAnim, unsigned int currentFrame);
		GLuint texGetHeight(unsigned int currentAnim, unsigned int currentFrame);
		GLuint texGetTextureID(unsigned int currentAnim, unsigned int currentFrame);
		void texGetFrameInfo(unsigned int currentAnim, unsigned int currentFrame, float &x, float &y, float &w, float &h, GLuint &frameTexID);
		bool containsTranslucency(unsigned int currentAnim, unsigned int currentFrame);

};

#endif
