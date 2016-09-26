#include "textureWrapper.hpp"
#include <SDL2/SDL.h>
#include <fstream>
#include <sstream>

bool textureWrapper::texLoad(std::string prgPath, std::string filePath){

	textureName = "";
	allFrames.clear();
	allAnimations.clear();


	std::ifstream texInfo((prgPath + filePath).c_str());
	std::string line;

	textureAnim tempAnim;     // Current texture animation being worked on
	int currentCommand = -1;  // The current multiline command type (-1 = none, 0 = texture, 1 = animation)
	unsigned int currentLine = 0;  // Current file line being read


	if(texInfo.is_open()){
		while(texInfo.good()){

			getline(texInfo, line);
			currentLine++;

			// Remove any comments from the line
			unsigned int commentPos = line.find("//");
			if(commentPos != std::string::npos){
				line.erase(commentPos);
			}
			// Remove any tabs from the line
			for(unsigned int d = 0; d < line.length(); d++){
				if(line.at(d) == '\t'){
					line.erase(d, 1);
				}
			}


			// Texture name
			if(line.length() >= 6 && line.substr(0, 5) == "name "){
				textureName = line.substr(5);


			// Close current multiline command
			}else if(line[0] == '}'){
				if(currentCommand == 0){
					// If a textureFrame was being worked on and has no subframes, add the default one
					if(allFrames.back().subframes.size() == 0){
						rectangle baseSubframe; baseSubframe.create(0, 0, allFrames.back().baseTexture.textureWidth,
					                                                      allFrames.back().baseTexture.textureHeight);
						allFrames.back().subframes.push_back(baseSubframe);
						std::vector<rectangle>(allFrames.back().subframes).swap(allFrames.back().subframes);
					}

				}else if(currentCommand == 1){
					// If a valid animation was being worked on, save it and continue
					if(tempAnim.frameIDs.size() > 0
					&& tempAnim.subframeIDs.size() == tempAnim.frameIDs.size()
					&& tempAnim.frameDelays.size() == tempAnim.frameIDs.size()){

						std::vector<unsigned int>(tempAnim.frameIDs).swap(tempAnim.frameIDs);
						std::vector<unsigned int>(tempAnim.subframeIDs).swap(tempAnim.subframeIDs);
						std::vector<float>(tempAnim.frameDelays).swap(tempAnim.frameDelays);
						allAnimations.push_back(tempAnim);
					}
				}

				currentCommand = -1;


			// New texture frame
			}else if(line.length() >= 9 && line.substr(0, 8) == "texture "){

				// A multiline command is already in progress; try to close it and continue
				if(currentCommand != -1){

					printf("Error loading texture wrapper:\nTrying to start a multiline command at line %u while another is already in progress; "
					       "I will attempt to close the current command.\n", currentLine);

					// If the multiline command is a texture...
					if(currentCommand == 0){
						// If a textureFrame was being worked on and has no subframes, add the default one
						if(allFrames.back().subframes.size() == 0){
							rectangle baseSubframe; baseSubframe.create(0, 0, allFrames.back().baseTexture.textureWidth,
						                                                      allFrames.back().baseTexture.textureHeight);
							allFrames.back().subframes.push_back(baseSubframe);
							std::vector<rectangle>(allFrames.back().subframes).swap(allFrames.back().subframes);
						}

					}else if(currentCommand == 1){
						// If a valid animation is being worked on, save it and continue
						if(tempAnim.frameIDs.size() > 0
						&& tempAnim.subframeIDs.size() == tempAnim.frameIDs.size()
						&& tempAnim.frameDelays.size() == tempAnim.frameIDs.size()){

							std::vector<unsigned int>(tempAnim.frameIDs).swap(tempAnim.frameIDs);
							std::vector<unsigned int>(tempAnim.subframeIDs).swap(tempAnim.subframeIDs);
							std::vector<float>(tempAnim.frameDelays).swap(tempAnim.frameDelays);
							allAnimations.push_back(tempAnim);
						}
					}

					currentCommand = -1;

				}


				// Create a new texture frame
				textureFrame tempFrame;

				unsigned int pathBegin = line.find('"') + 1;
				std::string texPath = line.substr(pathBegin, line.rfind('"') - pathBegin);

				if(tempFrame.baseTexture.createTexture((prgPath + texPath).c_str())){
					allFrames.push_back(tempFrame);  // Record its position and add it to the allFrames vector
					if(line.rfind('{') != std::string::npos){  // Check if the command spans multiple lines (it contains an opening brace)
						currentCommand = 0;
					}else{  // If it doesn't, add the default subframe
						rectangle baseSubframe; baseSubframe.create(0, 0, allFrames.back().baseTexture.textureWidth,
						                                                  allFrames.back().baseTexture.textureHeight);
						allFrames.back().subframes.push_back(baseSubframe);
						std::vector<rectangle>(allFrames.back().subframes).swap(allFrames.back().subframes);
					}
				}


			// New subframe
			}else if(line.length() >= 16 && line.substr(0, 9) == "subframe "){

				if(currentCommand == 0){

					// Replace each of the three /'s in the line with a space so parsing with ifstream is easier
					for(unsigned int d = 0; d < line.length(); d++){
						if(line.at(d) == '/'){
							line.replace(d, 1, " ");
						}
					}

					// Create a new subframe and add it to the current texture frame
					unsigned int dimensions[4];
					std::istringstream(line.substr(9)) >> dimensions[0] >> dimensions[1]
					                                   >> dimensions[2] >> dimensions[3];
					rectangle newSubframe; newSubframe.create(dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
					allFrames.back().subframes.push_back(newSubframe);
					std::vector<rectangle>(allFrames.back().subframes).swap(allFrames.back().subframes);

				}else{
					printf("Error loading texture wrapper:\nTexture sub-command invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// Load normal map
			}else if(line.length() >= 8 && line.substr(0, 7) == "normal "){
				if(currentCommand == 0){
					allFrames.back().normalTexture.createTexture((prgPath + line.substr(7)).c_str());
				}else{
					printf("Error loading texture wrapper:\nTexture sub-command invoked on line %u without specifying a multiline texture.\n", currentLine);
				}


			// New texture animation
			}else if(line.length() >= 9 && line.substr(0, 9) == "animation"){

				// Reset tempAnim
				tempAnim.loop = false;
				tempAnim.frameIDs.clear();
				tempAnim.subframeIDs.clear();
				tempAnim.frameDelays.clear();
				currentCommand = 1;


			// Make the current animation loop
			}else if(line.length() >= 4 && line.substr(0, 4) == "loop"){
				if(currentCommand == 1){
					tempAnim.loop = true;
				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command invoked on line %u without specifying an animation.\n", currentLine);
				}

			// New animation frame
			}else if(line.length() >= 11 && line.substr(0, 6) == "frame "){

				if(currentCommand == 1){

					// Replace each of the two /'s in the line with a space so parsing with ifstream is easier
					for(unsigned int d = 0; d < line.length(); d++){
						if(line.at(d) == '/'){
							line.replace(d, 1, " ");
						}
					}

					// Parse frame information
					unsigned int frameID, subframeID;
					float frameDelay;
					std::istringstream(line.substr(6)) >> frameID >> subframeID >> frameDelay;

					// Validate the frame information
					if(frameID < allFrames.size() && subframeID < allFrames.at(frameID).subframes.size()){
						tempAnim.frameIDs.push_back(frameID);
						tempAnim.subframeIDs.push_back(subframeID);
						tempAnim.frameDelays.push_back(frameDelay);
					}

				}else{
					printf("Error loading texture wrapper:\nAnimation sub-command invoked on line %u without specifying an animation.\n", currentLine);
				}

			}

		}

		texInfo.close();

	}else{
		printf("Error loading texture wrapper:\nCouldn't open %s\n", (prgPath + filePath).c_str());
		return false;
	}


	if(allFrames.size() == 0){

		printf("Error loading texture wrapper:\nNo textures were loaded.\n");
		return false;

	}else if(allFrames.back().subframes.size() == 0){

		rectangle baseSubframe; baseSubframe.create(0, 0, allFrames.back().baseTexture.textureWidth,
		                                                  allFrames.back().baseTexture.textureHeight);
		allFrames.back().subframes.push_back(baseSubframe);
		std::vector<rectangle>(allFrames.back().subframes).swap(allFrames.back().subframes);

	}

	if(allAnimations.size() == 0){
		// If an animation was being worked on, add it
		if(tempAnim.frameIDs.size() > 0 && tempAnim.subframeIDs.size() == tempAnim.frameIDs.size() &&
		   tempAnim.frameDelays.size() == tempAnim.frameIDs.size()){

			std::vector<unsigned int>(tempAnim.frameIDs).swap(tempAnim.frameIDs);
			std::vector<unsigned int>(tempAnim.subframeIDs).swap(tempAnim.subframeIDs);
			std::vector<float>(tempAnim.frameDelays).swap(tempAnim.frameDelays);
			allAnimations.push_back(tempAnim);

		}else{  // Otherwise add the default animation

			textureAnim defaultAnim;
			defaultAnim.loop = false;
			defaultAnim.frameIDs.push_back(0);
			defaultAnim.subframeIDs.push_back(0);
			defaultAnim.frameDelays.push_back(0);
			allAnimations.push_back(defaultAnim);

		}
	}

	// If no name was given, generate one based off the file name
	if(textureName == ""){
		unsigned int nameLastSlash = filePath.rfind('\\') + 1;
		textureName = filePath.substr(nameLastSlash, filePath.rfind('.') - nameLastSlash);
	}
	std::vector<textureFrame>(allFrames).swap(allFrames);
	std::vector<textureAnim>(allAnimations).swap(allAnimations);
	return true;

}

void textureWrapper::texAnimate(float speedMod, unsigned int &currentAnim, unsigned int &currentFrame, float &frameProgress){

	// Only animate the texture if the current animation is valid,
	// has more than one frame and can still be animated
	if(currentAnim < allAnimations.size() && allAnimations.at(currentAnim).frameIDs.size() > 1 &&
	   (allAnimations.at(currentAnim).loop || currentFrame < allAnimations.at(currentAnim).frameIDs.size() - 1)){

		if(currentFrame < allAnimations.at(currentAnim).frameIDs.size() &&   // Make sure the current frame is valid...
		   allAnimations.at(currentAnim).frameDelays.at(currentFrame) > 0){  // ...and that the frame delay is greater than zero

			// Get the current tick
			float currentTick = SDL_GetTicks() * speedMod;

			/* While the difference between currentTick and the frameProgress is greater than the
			desired delay period and the texture can still be animated, advance the animation */
			while((currentTick - frameProgress) >= allAnimations.at(currentAnim).frameDelays.at(currentFrame) &&
			      (allAnimations.at(currentAnim).loop || currentFrame < allAnimations.at(currentAnim).frameIDs.size() - 1)){

				// Add the delay to frameProgress and advance the animation
				frameProgress += allAnimations.at(currentAnim).frameDelays.at(currentFrame);
				currentFrame++;

				// Reset currentFrame if it gets too high
				if(currentFrame == allAnimations.at(currentAnim).frameIDs.size()){
					if(!allAnimations.at(currentAnim).loop){
						currentFrame = allAnimations.at(currentAnim).frameIDs.size() - 1;
					}else{
						currentFrame = 0;
					}
				}

			}

		}else{  // If it is too high, set it back

			// Only reset the animation if it can loop
			if(!allAnimations.at(currentAnim).loop){
				currentFrame = allAnimations.at(currentAnim).frameIDs.size() - 1;
			}else{
				currentFrame = 0;
			}
			frameProgress = SDL_GetTicks();

		}

	}else{

		if(currentAnim >= allAnimations.size()){
			currentAnim = 0;
		}
		if(currentFrame >= allAnimations.at(currentAnim).frameIDs.size()){
			if(!allAnimations.at(currentAnim).loop){
				currentFrame = allAnimations.at(currentAnim).frameIDs.size() - 1;
			}else{
				currentFrame = 0;
			}
			frameProgress = SDL_GetTicks();
		}

	}

}

GLuint textureWrapper::texGetWidth(unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < allAnimations.size() && currentFrame < allAnimations.at(currentAnim).frameIDs.size()){
		return allFrames.at(allAnimations.at(currentAnim).frameIDs.at(currentFrame)).baseTexture.textureWidth;
	}
	return 0;
}

GLuint textureWrapper::texGetHeight(unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < allAnimations.size() && currentFrame < allAnimations.at(currentAnim).frameIDs.size()){
		return allFrames.at(allAnimations.at(currentAnim).frameIDs.at(currentFrame)).baseTexture.textureHeight;
	}
	return 0;
}

GLuint textureWrapper::texGetTextureID(unsigned int currentAnim, unsigned int currentFrame){
	if(currentAnim < allAnimations.size() && currentFrame < allAnimations.at(currentAnim).frameIDs.size()){
		return allFrames.at(allAnimations.at(currentAnim).frameIDs.at(currentFrame)).baseTexture.textureID;
	}
	return 0;
}

void textureWrapper::texGetFrameInfo(unsigned int currentAnim, unsigned int currentFrame, float &x, float &y, float &w, float &h, GLuint &frameTexID){

	// Check if currentAnim and currentFrame are valid
	if(currentAnim < allAnimations.size() && currentFrame < allAnimations.at(currentAnim).frameIDs.size()){

		unsigned int currentFrameID = allAnimations.at(currentAnim).frameIDs.at(currentFrame);
		unsigned int currentSubframeID = allAnimations.at(currentAnim).subframeIDs.at(currentFrame);

		x = allFrames.at(currentFrameID).subframes.at(currentSubframeID).x;
		y = allFrames.at(currentFrameID).subframes.at(currentSubframeID).y;
		w = allFrames.at(currentFrameID).subframes.at(currentSubframeID).w;
		h = allFrames.at(currentFrameID).subframes.at(currentSubframeID).h;
		frameTexID = allFrames.at(currentFrameID).baseTexture.textureID;

	}else{

		x = 0;
		y = 0;
		w = 0;
		h = 0;
		frameTexID = 0;

	}

}

bool textureWrapper::containsTranslucency(unsigned int currentAnim, unsigned int currentFrame){

	if(currentAnim < allAnimations.size() && currentFrame < allAnimations.at(currentAnim).frameIDs.size()){

		unsigned int currentFrameID = allAnimations.at(currentAnim).frameIDs.at(currentFrame);
		return allFrames.at(currentFrameID).baseTexture.translucent;

	}

	return false;

}
