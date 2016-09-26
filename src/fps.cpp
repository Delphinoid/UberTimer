#include "fps.hpp"
#include <SDL2/SDL.h>

void fps::start(float dFps, float cFps){
	desiredFps = dFps;
	capFps = cFps;
	startTick = SDL_GetTicks();
	totalFrames = 0;
}

void fps::update(){
	totalFrames++;
	frameTick = SDL_GetTicks();
	actualFps = (totalFrames / (float)(frameTick - startTick)) * 1000;  // Actual fps = frames per millisecond multiplied by 1,000
	fpsMod = desiredFps / actualFps;
}

void fps::delay(){
	if(actualFps > capFps || capFps <= 0){
		//while(SDL_GetTicks() < frameTick + (1000 / capFps)){}  // Pauses until current tick is not less than the first tick of the frame plus how long the frame should be displayed
		//while(SDL_GetTicks() < startTick + totalFrames * (1000 / capFps)){}  // Same as above, slightly more accurate but slightly slower
		SDL_Delay((1000 / capFps) - (frameTick - SDL_GetTicks()));  // Milliseconds per frame minus how long the frame took (SDL_Delay is best for delays over 10 milliseconds)
	}
}
