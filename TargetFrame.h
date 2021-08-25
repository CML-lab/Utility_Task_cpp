#ifndef TGTFRAME_H
#define TGTFRAME_H
#pragma once

#include "SDL.h"

// Data type used by InputDevice to return input data
struct TargetFrame
{
	char fname[50];

	int trial;
	Uint32 time;

	int surebet;
	int gamble1;
	int gamble2;

	int lat;

	int choice;

	int outcome;
	int score;

};

#endif
