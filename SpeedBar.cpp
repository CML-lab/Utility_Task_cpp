#include <cmath>
#include <limits>
#include <sstream>
#include <iostream>
#include <fstream>
#include <istream>

#include "SpeedBar.h"
#include "Region2D.h"

void SpeedBar::MakeSpeedBar(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat framemin, GLfloat framemax, GLfloat min, GLfloat max, GLfloat tick, GLchar or, bool labels)
{
	
	float barlen;
	
	speed = 0;
	frameMaxSpeed = framemax;
	frameMinSpeed = framemin;
	maxSpeed = max;
	minSpeed = min;
	xpos = x-w/2; //x is centered, xpos is the lower left corner
	ypos = y-h/2; //y is centered, ypos is the lower left corner
	width = w; // width on screen
	height = h; // height on screen
	tickint = tick;
	orient = or;
	dolabels = labels;

	TTF_Font* font = TTF_OpenFont("Resources/arial.ttf", 20);
	SDL_Color textColor = {0, 0, 0};

	if (orient == 'v')
	{
		
		frame.SetNSides(4);
		frame.SetRectDims(width, height);
		frame.SetRegionCenter(xpos, ypos);

		if (tickint > 0)
		{
			nticks = floor((frameMaxSpeed-frameMinSpeed)/tickint)+1;
			nticks = (nticks > 11 ? 11 : nticks);
			float vert[1][6];
			vert[0][0] = -(width/2.0f)*1.25f;
			vert[0][1] = 0.0f;
			vert[0][2] = (width/2.0f)*1.25f;
			vert[0][3] = 0.0f;
			vert[0][4] = 0.0f;
			vert[0][5] = 0.0f;
			float blkColor[3] = {0.0f, 0.0f, 0.0f};
	
			for (int a = 0; a < nticks; a++)
			{
				ticks[a].SetNVerts(1);
				ticks[a].SetPathVerts(vert);
				ticks[a].SetPathColor(blkColor);
				ticks[a].SetPathWidth(1);
				ticks[a].SetPathCenter(x,ypos+height/(nticks-1)*a);
				ticks[a].Off();

			}
		}
		else
			nticks = 0;

		bar.SetNSides(4);

		barlen = (speed-frameMinSpeed)*height/(frameMaxSpeed-frameMinSpeed);
		barlen = (barlen<0 ? 0 : barlen);			//restrict the minimum length to be 0
		barlen = (barlen>height ? height : barlen);	//restrict the maximum length to be the frame length

		bar.SetRectDims(width/2.0f, barlen);
		bar.SetRegionCenter(xpos+width/4.0f, ypos);


		speedregion.SetNSides(4);
		speedregion.SetRectDims(width*1.05, (maxSpeed-minSpeed)*height/(frameMaxSpeed-frameMinSpeed));
		speedregion.SetRegionCenter(xpos-width*0.025f, ypos+((minSpeed-frameMinSpeed)*height/(frameMaxSpeed-frameMinSpeed)));

	}
	else  //default is a horizontal bar/frame
	{
		
		frame.SetNSides(4);
		frame.SetRectDims(width, height);
		frame.SetRegionCenter(xpos, ypos);

		if (tickint > 0)
		{
			nticks = floor((frameMaxSpeed-frameMinSpeed)/tickint)+1;
			nticks = (nticks > 11 ? 11 : nticks);
			float vert[1][6];
			vert[0][0] = 0.0f;
			vert[0][1] = (height/2.0f)*1.25f;
			vert[0][2] = 0.0f;
			vert[0][3] = -(height/2.0f)*1.25f;
			vert[0][4] = 0.0f;
			vert[0][5] = 0.0f;
			float blkColor[3] = {0.0f, 0.0f, 0.0f};
	
			for (int a = 0; a < nticks; a++)
			{
				ticks[a].SetNVerts(1);
				ticks[a].SetPathVerts(vert);
				ticks[a].SetPathColor(blkColor);
				ticks[a].SetPathWidth(1);
				ticks[a].SetPathCenter(xpos+(width/(nticks-1))*a,y);
				ticks[a].Off();
			}
		}
		else
			nticks = 0;


		if (dolabels)
		{
			std::stringstream tmplabel;
			for (int a = 0; a < nticks; a++)
			{
				tmplabel << (frameMinSpeed + tickint*a);
				std::cerr << "  Label: " << tmplabel.str().c_str() << std::endl;
				ticklabels[a] = new Image(TTF_RenderText_Blended(font, tmplabel.str().c_str(), textColor));
				ticklabels[a]->Off();
				tmplabel.str("");
			}
		}


		bar.SetNSides(4);

		barlen = (speed-frameMinSpeed)*width/(frameMaxSpeed-frameMinSpeed);
		barlen = (barlen<0 ? 0 : barlen);			//restrict the minimum length to be 0
		barlen = (barlen>width ? width : barlen);	//restrict the maximum length to be the frame length

		bar.SetRectDims(barlen, height/2.0f);
		bar.SetRegionCenter(xpos, ypos+height/4.0f);


		speedregion.SetNSides(4);
		speedregion.SetRectDims((maxSpeed-minSpeed)*width/(frameMaxSpeed-frameMinSpeed), height*1.05f);
		speedregion.SetRegionCenter(xpos+((minSpeed-frameMinSpeed)*width/(frameMaxSpeed-frameMinSpeed)), ypos-height*0.025f);

	}

	frameColor[0] = 0.9f;
	frameColor[1] = 0.9f;
	frameColor[2] = 0.9f;

	barColor[0] = 0.0f;
	barColor[1] = 1.0f;
	barColor[2] = 1.0f;

	speedregionColor[0] = 0.5f;
	speedregionColor[1] = 0.5f;
	speedregionColor[2] = 0.5f;

	frame.SetRegionColor(frameColor);
	bar.SetRegionColor(barColor);
	speedregion.SetRegionColor(speedregionColor);

}

void SpeedBar::SetSpeedBounds(GLfloat min, GLfloat max)
{
	maxSpeed = max;
	minSpeed = min;
}

void SpeedBar::SetFrameBounds(GLfloat min, GLfloat max)
{
	frameMaxSpeed = max;
	frameMinSpeed = min;
}

void SpeedBar::UpdateSpeed(GLfloat currSpeed)
{
	speed = currSpeed;

	float barlen;

	if (orient == 'v')
	{
		barlen = (speed-frameMinSpeed)*height/(frameMaxSpeed-frameMinSpeed);
		barlen = (barlen<0 ? 0 : barlen);			//restrict the minimum length to be 0
		barlen = (barlen>height ? height : barlen);	//restrict the maximum length to be the frame length

		bar.SetRectDims(width/2.0f, barlen);
		bar.SetRegionCenter(xpos+width/4.0f, ypos);

	}
	else  //default is a horizontal bar/frame
	{
		barlen = (speed-frameMinSpeed)*width/(frameMaxSpeed-frameMinSpeed);
		barlen = (barlen<0 ? 0 : barlen);			//restrict the minimum length to be 0
		barlen = (barlen>width ? width : barlen);	//restrict the maximum length to be the frame length

		bar.SetRectDims(barlen, height/2.0f);
		bar.SetRegionCenter(xpos, ypos+height/4.0f);
	}

}

void SpeedBar::On()
{
	frame.On();
	bar.On();
	speedregion.On();

	for (int a = 0; a < nticks; a++)
	{
		ticks[a].On();
		if (dolabels)
			ticklabels[a]->On();
	}


}

void SpeedBar::Off()
{
	frame.Off();
	bar.Off();
	speedregion.Off();

	for (int a = 0; a < nticks; a++)
	{
		ticks[a].Off();
		if (dolabels)
			ticklabels[a]->Off();
	}

}

void SpeedBar::Draw()
{
	frame.Draw();
	speedregion.Draw();
	bar.Draw();

	for (int a = 0; a < nticks; a++)
	{
		ticks[a].Draw();

		if (orient == 'v' && dolabels)
			ticklabels[a]->DrawAlign(ticks[a].GetPathCenteredVert(0,2)+width/4,ticks[a].GetPathCenteredVert(0,3),ticklabels[a]->GetWidth(),ticklabels[a]->GetHeight(),3);
		else if (orient == 'h' && dolabels)//orient == 'h'
			ticklabels[a]->DrawAlign(ticks[a].GetPathCenteredVert(0,2),ticks[a].GetPathCenteredVert(0,3)-height/4,ticklabels[a]->GetWidth(),ticklabels[a]->GetHeight(),2);
	}
}


//destructor
SpeedBar::~SpeedBar()
{
	frame.~Region2D();
	speedregion.~Region2D();
	bar.~Region2D();

	for (int a = 0; a < nticks; a++)
	{
		ticks[a].~Path2D();

		if (dolabels)
			ticklabels[a]->~Image();
	}
	

}