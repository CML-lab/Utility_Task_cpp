#ifndef SPEEDBAR_H
#define SPEEDBAR_H
#pragma once

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_ttf.h"
#include "Region2D.h"
#include "Path2D.h"
#include "Image.h"

class SpeedBar
{
private:
	GLfloat speed; // current speed
	GLfloat frameMaxSpeed;  // maximum speed represented on bar
	GLfloat frameMinSpeed;  // minimum speed represented on bar
	GLfloat maxSpeed; // maximum desired speed
	GLfloat minSpeed; // minimum desired speed
	GLfloat frameColor[3]; // color of bar frame
	GLfloat barColor[3]; // color of bar
	GLfloat speedregionColor[3]; //color of desired speed region

	GLfloat xpos;  //x position of lower left corner of bar frame
	GLfloat ypos;  //y position of lower left corner of bar frame
	GLfloat width;  //width of bar frame
	GLfloat height; //height of bar frame

	GLfloat tickint;
	GLint nticks;
	Image* ticklabels[11];
	bool dolabels;


	char orient; //orientation of bar frame ('h', horizontal or 'v', vertical)

	Region2D frame;
	Region2D bar;
	Region2D speedregion;
	Path2D ticks[11];

public:
	// constructor
	void MakeSpeedBar(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat framemin, GLfloat framemax, GLfloat min, GLfloat max, GLfloat tick, GLchar or, bool labels);

	// destructor
	~SpeedBar();

	void SetFrameBounds(GLfloat max, GLfloat min);
	void SetSpeedBounds(GLfloat max, GLfloat min);
	void UpdateSpeed(GLfloat currSpeed);

	void On();
	void Off();

	void Draw();

};

#endif