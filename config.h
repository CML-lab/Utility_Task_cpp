#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#include "SDL_opengl.h"

// Configurable constants

//****UNCOMMENT ONLY ONE CODE BLOCK BELOW****DO NOT TOUCH OTHER SECTIONS OF CODE****


//define the trial-table
#define TRIALFILE "utilitytable.txt"


//**********************************************************************************



//screen dimensions


#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT  1080

/*
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT  1024
*/

#define SCREEN_BPP  32
#define WINDOWED  false
#define MIRRORED  false
// Physical dimensions of the screen in meters
#define PHYSICAL_WIDTH  SCREEN_WIDTH
#define PHYSICAL_HEIGHT  SCREEN_HEIGHT
// screen ratio, in meters per pixel
#define PHYSICAL_RATIO  (PHYSICAL_WIDTH / SCREEN_WIDTH)

//target dimensions, pixels
#define SELECTION_RADIUS  20.0f


//time to wait between successive trials
#define WAITITI 20000
#define TRIALDUR -1
#define ITI 800

//define pi as a float
#define PI 3.14159265358979f

#define SAMPRATE 1.0


#endif
