#include "config.h"
#include "MouseInput.h"
#include <cmath>

InputFrame MouseInput::GetFrame()
{
	InputFrame input;
	input.x = x;
	input.y = y;
	
	input.btn = btn;

	input.time = mtime;

	return input;
}

void MouseInput::ProcessEvent(SDL_Event event)
{
	GLfloat scale = PHYSICAL_WIDTH/10;

	if (event.type == SDL_MOUSEMOTION)
	{
		// Convert position data to meters
		x = (GLfloat)event.motion.x * PHYSICAL_RATIO;
		y = (GLfloat)(SCREEN_HEIGHT - event.motion.y) * PHYSICAL_RATIO;

		btn = 0;

	}

	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		// Convert position data to meters
		x = (GLfloat)event.button.x * PHYSICAL_RATIO;
		y = (GLfloat)(SCREEN_HEIGHT - event.button.y) * PHYSICAL_RATIO;

		//if (event.button.button == 1)
		btn = event.button.button;
		mtime = event.button.timestamp;

	}

	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		// Convert position data to meters
		x = (GLfloat)event.button.x * PHYSICAL_RATIO;
		y = (GLfloat)(SCREEN_HEIGHT - event.button.y) * PHYSICAL_RATIO;

		//if (event.button.button == 1)
		btn = 0;
		mtime = event.button.timestamp;

	}

}