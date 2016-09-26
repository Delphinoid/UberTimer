#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include "gfxProgram.hpp"
#include <windows.h>

extern gfxProgram gfxPrg;
extern HHOOK hook;
extern KBDLLHOOKSTRUCT keyboardHookStruct;
extern unsigned int resetKey;
extern unsigned int pauseKey;
extern unsigned int abortKey;
extern unsigned int uberStartTick;
extern unsigned int uberPauseTick;
extern bool uberPaused;
extern bool prgRunning;

#endif
