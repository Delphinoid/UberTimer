#include <SDL2/SDL.h>
#include "globalVariables.hpp"

LRESULT __stdcall keyboardEventCallback(int nCode, WPARAM wParam, LPARAM lParam){

    if(nCode >= 0){
        if(wParam == WM_KEYDOWN){

            keyboardHookStruct = *((KBDLLHOOKSTRUCT*)lParam);

            if(keyboardHookStruct.scanCode == resetKey){
				uberStartTick = SDL_GetTicks();
				gfxPrg.uberPercent  = 0;
				gfxPrg.kritzPercent = 0;

            }
            if(keyboardHookStruct.scanCode == pauseKey){
            	uberPaused = !uberPaused;
            	if(uberPaused){
					uberPauseTick = SDL_GetTicks() - uberStartTick;
            	}else{
					uberStartTick = SDL_GetTicks() - uberPauseTick;
            	}

            }
            if(keyboardHookStruct.scanCode == abortKey){
            	prgRunning = false;

            }

        }
    }

    return CallNextHookEx(hook, nCode, wParam, lParam);

}
