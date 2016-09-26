#include "gfxProgram.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "sprite.hpp"
#include "fps.hpp"
#include "globalVariables.hpp"

#define UBER_LENGTH 40000
#define KRITZ_LENGTH 32000

gfxProgram gfxPrg;
HHOOK hook;
KBDLLHOOKSTRUCT keyboardHookStruct;
unsigned int resetKey = 0x30;
unsigned int pauseKey = 0x30;
unsigned int abortKey = 0x53;
unsigned int uberStartTick = 0;
unsigned int uberPauseTick = 0;
bool uberPaused = false;
bool prgRunning = true;

void loadConfig(const char *cfgPath, unsigned int &windowX, unsigned int &windowY, unsigned int &windowW, unsigned int &windowH);
LRESULT __stdcall keyboardEventCallback(int nCode, WPARAM wParam, LPARAM lParam);
void renderScene(gfxProgram &gfxPrg, std::vector<sprite> &allSprites);

int main(int argc, char *argv[]){

	// Get program path
	std::string path = argv[0];
	path.erase(path.rfind('\\') + 1);  // Removes program name (everything after the last backslash) from the path

	// Load config
	unsigned int windowX = 0;
	unsigned int windowY = 0;
	unsigned int windowW = DEFAULT_WIDTH;
	unsigned int windowH = DEFAULT_HEIGHT;
	loadConfig((path + "Resources\\config.txt").c_str(), windowX, windowY, windowW, windowH);

	// Set up graphics libraries
	if(!gfxPrg.initPrg(windowX, windowY, windowW, windowH, (path + "Resources\\Shaders\\vertexShader.vsh"), (path + "Resources\\Shaders\\fragmentShader.fsh"))){
		return 1;
	}

	// Create window handle
	HWND hWindow = FindWindow(NULL, "Uber Timer");

	// Creates a hook to monitor the system for keyboard events
	if(!(hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardEventCallback, NULL, 0))){
        printf("Couldn't create a keyboard hook!\n");
        return 1;
    }

	// Set up projection matrix
	gfxPrg.projectionMatrix = glm::ortho(0.f, (float)windowW, (float)windowH, 0.f);
	glUniformMatrix4fv(gfxPrg.mvpMatrixID, 1, GL_FALSE, &gfxPrg.projectionMatrix[0][0]);

	// Load textures
	textureWrapper digitTex;
	digitTex.texLoad(path, "Resources\\Textures\\Animated\\Digit.tdt");

	// Load sprites
	std::vector<sprite> allSprites;

	sprite tempSprite;
	tempSprite.sprTexture = &digitTex;
	tempSprite.currentAnim = 1;
	tempSprite.width = windowW / 4;
	tempSprite.height = windowH / 2;
	allSprites.push_back(tempSprite);
	tempSprite.currentAnim = 0;
	tempSprite.position.x = tempSprite.width;
	allSprites.push_back(tempSprite);
	tempSprite.position.x = tempSprite.position.x + tempSprite.width;
	allSprites.push_back(tempSprite);
	tempSprite.currentAnim = 11;
	tempSprite.position.x = tempSprite.position.x + tempSprite.width;
	allSprites.push_back(tempSprite);

	tempSprite.currentAnim = 1;
	tempSprite.position.x = 0;
	tempSprite.position.y = tempSprite.height;
	allSprites.push_back(tempSprite);
	tempSprite.currentAnim = 0;
	tempSprite.position.x = tempSprite.width;
	allSprites.push_back(tempSprite);
	tempSprite.position.x = tempSprite.position.x + tempSprite.width;
	allSprites.push_back(tempSprite);
	tempSprite.currentAnim = 11;
	tempSprite.position.x = tempSprite.position.x + tempSprite.width;
	allSprites.push_back(tempSprite);

	 // Load sound effects
    Mix_Chunk *uberSfx  = Mix_LoadWAV((path + "Resources\\Audio\\uberReady.wav").c_str());
	Mix_Chunk *kritzSfx = Mix_LoadWAV((path + "Resources\\Audio\\kritzReady.wav").c_str());

	// Bind the VAO, VBO and texture
	glBindVertexArray(gfxPrg.spriteVaoID);
	glBindBuffer(GL_ARRAY_BUFFER, gfxPrg.spriteVboID);
	glBindTexture(GL_TEXTURE_2D, digitTex.texGetTextureID(0, 0));


	SDL_Event prgEventHandler;
	fps fpsHandler; fpsHandler.start(61, 61);

	uberStartTick = SDL_GetTicks();

	// Main loop
	while(prgRunning){

		/* Update FPS */
		fpsHandler.update();

		/* Bring window to the front without stealing focus */
		SetWindowPos(hWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		/* Detect application exit */
		SDL_PollEvent(&prgEventHandler);
		if(prgEventHandler.type == SDL_QUIT){
			prgRunning = false;
		}

		/* Update uberPercent */
		if(!uberPaused){
			if(gfxPrg.uberPercent == 100){
				Mix_PlayChannel(0, uberSfx, 0);
				gfxPrg.uberPercent++;
			}else if(gfxPrg.uberPercent < 100){
				gfxPrg.uberPercent = (unsigned int)((float)(SDL_GetTicks() - uberStartTick) / UBER_LENGTH  * 100);
			}

			if(gfxPrg.kritzPercent == 100){
				Mix_PlayChannel(0, kritzSfx, 0);
				gfxPrg.kritzPercent++;
			}else if(gfxPrg.kritzPercent < 100){
				gfxPrg.kritzPercent = (unsigned int)((float)(SDL_GetTicks() - uberStartTick) / KRITZ_LENGTH  * 100);
			}
		}

		/* Render the scene */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
		renderScene(gfxPrg, allSprites);                     // Render
		SDL_GL_SwapWindow(gfxPrg.window);                    // Update

		/* Pause the program to maintain a constant FPS */
		fpsHandler.delay();

	}


	UnhookWindowsHookEx(hook);
	Mix_FreeChunk(uberSfx);
	Mix_FreeChunk(kritzSfx);

	return 0;

}
