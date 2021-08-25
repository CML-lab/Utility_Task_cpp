#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip> 
#include <fstream>
#include <istream>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "TargetFrame.h"

#include "MouseInput.h" 
#include "DataWriter.h"

#include "Object2D.h"
#include "Sound.h"
#include "Circle.h"
#include "Region2D.h"

#include <gl/GL.h>
#include <gl/GLU.h>
/*
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "SDL2_image.lib")
*/

/* 
	Evaluation of Subjective Reward Utility 
	   -Modified from Iowa Gambling Task, 11/2/2017
	   -General functionality includes loading a trial table and executing the experiment
	   -All FOB code has been removed, in favor of keyboard/mouse input

*/


//state machine
enum GameState
{
	Idle = 0x01,     //00001
	WaitDisp = 0x02, //00010
	ShowDisp = 0x04, //00100
	Ending = 0x0F,   //01111
	Finished = 0x10  //10000
};



SDL_Event event;
SDL_Window *screen = NULL;
SDL_GLContext glcontext = NULL;

InputDevice* controller;

Image* choicetext[4];
Image* text = NULL;
Image* pausetext = NULL;
Image* trialnum = NULL;
Image* instructtext = NULL;

Region2D choiceregions[4];

TTF_Font* font = NULL;
TTF_Font* trialnumfont = NULL;
TTF_Font* choicefont = NULL;
SDL_Color textColor = {0, 0, 0};
SDL_Color respColor = {255, 0, 0};
float greenColor[3] = {0.23f, 0.65f, 0.23f};
float greenselColor[3] = {0.53f, 0.95f, 0.53f};
float blueColor[3] = {0.0f, 0.0f, 1.0f};
float blueselColor[3] = {0.4f, 0.4f, 1.0f};
Sound* scorebeep = NULL;
Sound* errorbeep = NULL;
Sound* startbeep = NULL;

DataWriter* writer = NULL;
GameState state;
Uint32 gameTimer;
Uint32 hoverTimer;
Uint32 latencyTimer;
Uint32 updateTimer;

//variables to keep track of what selection the user input
int choiceinput;  //set to 0 for no choice; 1 for sure bet, 2 for gamble

//variable to mandate a write out to the data stream
bool manualwritedata = false;

// Trial table structure, to keep track of all the parameters for each trial of the experiment
typedef struct {
	int surebet;				// value of sure bet
	int gamble1;				// value of gamble-win
	int gamble2;				// value of gamble-lose
} TRTBL;

#define TRTBL_SIZE 120
TRTBL trtbl [TRTBL_SIZE];

int NTRIALS = 0;
int CurTrial = 0;

#define curtr trtbl[CurTrial]

//target, trace, and path structure; keep track of what is being displayed
TargetFrame Target;

int score;

//records mouse button press data
bool mousebtnhit = false;
int kbhit = 0;
int charc = 1;
int charm = 2;

// Initializes everything and returns true if there were no errors
bool init();
// Sets up OpenGL
void setup_opengl();
// Performs closing operations
void clean_up();
// Draws objects on the screen
void draw_screen();

//file to load in trial table
int LoadTrFile(char *filename);

// Update loop (state machine)
void game_update();

bool pause = true; //flag to pause the experiment; start paused.
bool quit = false;

int main(int argc, char* args[])
{
	int a = 0;

	//redirect stderr output to a file
	freopen( "./Debug/errorlog.txt", "w", stderr); 

	std::cerr << "Start main." << std::endl;

	//HIGH_PRIORITY_CLASS
	if (SetPriorityClass(GetCurrentProcess(),ABOVE_NORMAL_PRIORITY_CLASS))
		std::cerr << "Promote process priority to Above Normal." << std::endl;
	else
		std::cerr << "Promote process priority failed." << std::endl;

	if (!init())
	{
		// There was an error during initialization
		std::cerr << "Initialization error." << std::endl;
		return 1;
	}
	
	//flush events
	while (SDL_PollEvent(&event))
	{}


	quit = false;
	
	while (!quit)
	{
		//game_update(); // Run the game loop

		bool kb_updated = false;
		bool mouse_updated = false;

		// Handle SDL events
		while (SDL_PollEvent(&event))
		{
			
			// See http://www.libsdl.org/docs/html/sdlevent.html for list of event types
			if (event.type == SDL_MOUSEMOTION)
			{
				mouse_updated = true; // Record mouse data if joystick is not connected
				MouseInput::ProcessEvent(event); // Pass this event to the MouseInput class to process
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				mouse_updated = true;
				MouseInput::ProcessEvent(event); // Pass this event to the MouseInput class to process
				//SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
			}
			else if (event.type == SDL_KEYDOWN)
			{
				kb_updated = true;

				// See http://www.libsdl.org/docs/html/sdlkey.html for Keysym definitions
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else if (event.key.keysym.sym == SDLK_p)
				{
					pause = true;
				}
				else if (event.key.keysym.sym == SDLK_r)
				{
					pause = false;
				}
				else if (event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_f)
				{
					Target.time = event.key.timestamp;
					choiceinput = 1;  //1 is 'd'
				}
				else if (event.key.keysym.sym == SDLK_m || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_j)
				{
					Target.time = event.key.timestamp;
					choiceinput = 2;  //2 is 'f'
				}

			}
			//else if (event.type == SDL_KEYUP ||  event.type == SDL_MOUSEBUTTONUP)
			//{
			//	kbhit = 0;
			//}
			else if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}


		if (!pause)
		{
			if ((CurTrial >= NTRIALS) && (state == Finished) && (SDL_GetTicks() - gameTimer >= 5000))
				quit = true;

			// Get data from input devices
			if (mouse_updated) // if there is a new frame of data
			{

				InputFrame i;
				i = controller->GetFrame();
				if(i.btn == SDL_BUTTON_LEFT)
					choiceinput = 1;
				else if (i.btn == SDL_BUTTON_RIGHT)
					choiceinput = 2;

				Target.time = i.time;

			}

			if (mouse_updated || kb_updated)
			{
				mouse_updated = false;
				kb_updated = false;

				//flush event queue
				while (SDL_PollEvent(&event))
				{}
			}

			if (manualwritedata)
			{
				writer->Record(0, Target);
				//choiceinput = 0;
				manualwritedata = false;
			}

			game_update(); // Run the game loop (state machine update)

			draw_screen();

			//SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
		}
		else  //paused program
			draw_screen();



		//note, since we are using mouse inputs, we don't want this loop to run constantly, that updates everything too 
		//   fast.  so, we will ask the system to wait for a short period of time (at least this long...).  if the screen refreshes
		//   at 60 Hz, this is a delay of about 15 msec, so we should ask the system to wait about half that long, or 7 msec.
		SDL_Delay(7);  //delay in msec
	}

	clean_up();

	return 0;
}


//function to read in the name of the trial table file, and then load that trial table
int LoadTrFile(char *fname)
{

	//std::cerr << "LoadTrFile begin." << std::endl;

	char tmpline[100] = ""; 
	int ntrials = 0;

	
	//read in the trial file name
	std::ifstream trfile(fname);

	if (!trfile)
	{
		std::cerr << "Cannot open input file." << std::endl;
		return -1;
		//exit(1);
	}
	else
		std::cerr << "Opened TrialFile " << TRIALFILE << std::endl;

	trfile.getline(tmpline,sizeof(tmpline),'\n');
	
	while(!trfile.eof())
	{
		sscanf(tmpline, "%d %d %d",&trtbl[ntrials].surebet,&trtbl[ntrials].gamble1, &trtbl[ntrials].gamble2); 
		ntrials++;
		trfile.getline(tmpline,sizeof(tmpline),'\n');
	}

	trfile.close();
	if(ntrials == 0)
	{
		std::cerr << "Empty input file." << std::endl;
		//exit(1);
		return -1;
	}
	return ntrials;
}



//initialization function - set up the experimental environment and load all relevant parameters/files
bool init()
{

	int a;
	char tmpstr[80];
	char fname[50] = TRIALFILE;

	//std::cerr << "Start init." << std::endl;

	// Initialize SDL, OpenGL, SDL_mixer, and SDL_ttf
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cerr << "SDL failed to intialize."  << std::endl;
		return false;
	}
	else
		std::cerr << "SDL initialized." << std::endl;

	screen = SDL_CreateWindow("Code Base SDL2",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | (WINDOWED ? 0 : SDL_WINDOW_FULLSCREEN)); //SCREEN_BPP,
	if (screen == NULL)
	{
		std::cerr << "Screen failed to build." << std::endl;
		return false;
	}
	else
	{
		glcontext = SDL_GL_CreateContext(screen);
		std::cerr << "Screen built." << std::endl;
	}

	SDL_GL_SetSwapInterval(0); //ask for immediate updates rather than syncing to vertical retrace

	setup_opengl();
	a = Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 512);  //initialize SDL_mixer
	if (a != 0)
	{
		std::cerr << "Audio failed to initialize." << std::endl;
		//return false;
	}
	else
		std::cerr << "Audio initialized." << std::endl;

	if (TTF_Init() == -1)
	{
		std::cerr << "Text failed to initialize." << std::endl;
		return false;
	}
	else
		std::cerr << "Text initialized." << std::endl;

	//turn off the computer cursor
	//SDL_ShowCursor(0);

	std::cerr << "Images loaded." << std::endl;

	float btw = PHYSICAL_WIDTH;
	float stimwidth = btw*1/6;  //160
	float bth = PHYSICAL_HEIGHT;
	float stimheight = bth*1/6;
	GLfloat polyverts[4][2];
	polyverts[0][0] = -stimwidth/2;
	polyverts[0][1] = -stimheight/2;
	polyverts[1][0] = stimwidth/2;
	polyverts[1][1] = -stimheight/2;
	polyverts[2][0] = stimwidth/2;
	polyverts[2][1] = stimheight/2;
	polyverts[3][0] = -stimwidth/2;
	polyverts[3][1] = stimheight/2;

	std::cerr << "Stim width: " << stimwidth << " out of " << btw << std::endl;

	for (a = 0; a < 2; a++)
	{
		choiceregions[a].SetNSides(4);
		choiceregions[a].SetRegionColor(greenColor);
		choiceregions[a].SetRegionVerts(polyverts);
		choiceregions[a].SetRegionCenter(PHYSICAL_WIDTH*(a+1)/3,PHYSICAL_HEIGHT/2);
		choiceregions[a].On();
	}


	//load sound files
	startbeep = new Sound("Users/User/Documents/Cpp code/Bez_path/Resources/beep1.wav");
	scorebeep = new Sound("Resources/coins.wav");
	errorbeep = new Sound("Resources/errorbeep.wav");


	//initialize mouse
	controller = new MouseInput();

	//load trial table from file
	NTRIALS = LoadTrFile(fname);
	//std::cerr << "Filename: " << fname << std::endl;
	
	if(NTRIALS == -1)
	{
		std::cerr << "Trial File did not load." << std::endl;
		return false;
	}
	else
		std::cerr << "Trial File loaded: " << NTRIALS << " trials found." << std::endl;

	//assign the data-output file name based on the trial-table name 
	std::string savfile;
	savfile.assign(fname);
	savfile.insert(savfile.rfind("."),"_data");

	std::strcpy(fname,savfile.c_str());

	std::cerr << "SavFileName: " << fname << std::endl;

	writer = new DataWriter(Target,fname);  //set up the data-output file


	/* To create text, call a render function from SDL_ttf and use it to create
	 * an Image object. See http://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.html#SEC42
	 * for a list of render functions.
	 */
	font = TTF_OpenFont("Resources/arial.ttf", 28);

	text = new Image(TTF_RenderText_Blended(font, "You won 0 points.", textColor));
	text->Off();

	pausetext = new Image(TTF_RenderText_Blended(font, "PAUSED", respColor));

	instructtext = new Image(TTF_RenderText_Blended(font, "Sure bet or 50/50 gamble?", textColor));
	instructtext->Off();

	choicefont = TTF_OpenFont("Resources/arial.ttf", 26);
	choicetext[0] = new Image(TTF_RenderText_Blended(choicefont, " ", textColor));
	choicetext[0]->Off();
	choicetext[1] = new Image(TTF_RenderText_Blended(choicefont, " ", textColor));
	choicetext[1]->Off();
	choicetext[2] = new Image(TTF_RenderText_Blended(choicefont, "or", textColor));
	choicetext[2]->Off();
	choicetext[3] = new Image(TTF_RenderText_Blended(choicefont, " ", textColor));
	choicetext[3]->Off();

	trialnumfont = TTF_OpenFont("Resources/arial.ttf", 12);
	trialnum = new Image(TTF_RenderText_Blended(trialnumfont, "1", textColor));
	trialnum->On();

	// Set the initial game state
	state = Idle; 

	updateTimer = SDL_GetTicks();
	gameTimer = updateTimer;

	std::cerr << "initialization complete." << std::endl;
	return true;
}


static void setup_opengl()
{
	
	glClearColor(255, 255, 255, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* The default coordinate system has (0, 0) at the bottom left. Width and
	 * height are in meters, defined by PHYSICAL_WIDTH and PHYSICAL_HEIGHT
	 * (config.h). If MIRRORED (config.h) is set to true, everything is flipped
	 * horizontally.
	 */
	glOrtho(MIRRORED ? PHYSICAL_WIDTH : 0, MIRRORED ? 0 : PHYSICAL_WIDTH,
		0, PHYSICAL_HEIGHT, -1.0f, 1.0f);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	
}


//end the program; clean up everything neatly.
void clean_up()
{
	
	delete text;
	delete pausetext;
	delete instructtext;
	for (int a = 0; a < 2; a++)
		delete choicetext[a];
	delete trialnum;
	delete writer;
	Mix_CloseAudio();
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
}

//control what is drawn to the screen
static void draw_screen()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw the base trace
	//BaseTrace->Draw();


	//draw region1 on the screen
	choiceregions[0].Draw();
	choicetext[0]->Draw(choiceregions[0].GetRegionCenterX(),choiceregions[0].GetRegionCenterY());

	//draw region2 on the screen
	choiceregions[1].Draw();
	choicetext[1]->Draw(choiceregions[1].GetRegionCenterX(),choiceregions[1].GetRegionCenterY()+(choicetext[2]->GetHeight())*1.3);
	choicetext[2]->Draw(choiceregions[1].GetRegionCenterX(),choiceregions[1].GetRegionCenterY());
	choicetext[3]->Draw(choiceregions[1].GetRegionCenterX(),choiceregions[1].GetRegionCenterY()-(choicetext[2]->GetHeight())*1.3);

	// draw trial number
	trialnum->Draw(PHYSICAL_WIDTH*23/24, PHYSICAL_HEIGHT*1/24);

	if (pause)
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glColor4f(0.9f,0.9f,0.9f,0.8f);

		glBegin(GL_TRIANGLE_FAN);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(PHYSICAL_WIDTH, 0.0f, 0.0f);
		glVertex3f(PHYSICAL_WIDTH, PHYSICAL_HEIGHT, 0.0f);
		glVertex3f(0.0f, PHYSICAL_HEIGHT, 0.0f);
		glEnd();

		glColor4f(1.0f, 1.0f, 1.0f,1.0f);

		pausetext->Draw(PHYSICAL_WIDTH/2, PHYSICAL_HEIGHT*11/16);
	}
	else
	{
		// Draw text - provide feedback
		instructtext->Draw(PHYSICAL_WIDTH/2, PHYSICAL_HEIGHT*11/16);
		text->Draw(PHYSICAL_WIDTH/2, PHYSICAL_HEIGHT*11/16);
	}

	SDL_GL_SwapWindow(screen);
	glFlush();

}

//game update loop - state machine controlling the status of the experiment
bool endtrial = false;
int trialdur;
int outcome;


void game_update()
{

	int a;

	switch (state)
	{
		case Idle:
			/* If player starts hovering over start marker, set state to Starting
			 * and store the time -- this is for new trials only!
			 */

			for (a = 0; a < 2; a++)
				choiceregions[a].Off();

			for (a = 0; a < 4; a++)
				choicetext[a]->Off();

			choiceinput = 0;
			outcome = 0;

			endtrial = false;

			//make sure we are on a valid trial number and have waited the ITI
			if( (CurTrial < NTRIALS) && (SDL_GetTicks()-gameTimer)>ITI) //(Object2D::Distance(jscursor, starttgt) < CURSOR_RADIUS)
			{
				text->Off();
				instructtext->On();
				
				Target.trial = CurTrial+1;
				Target.surebet = -1;
				Target.gamble1 = -1;
				Target.gamble2 = -1;
				Target.choice = -1;
				Target.outcome = -1;
				Target.score = score;
				Target.lat = -99;

				for (a = 0; a < 2; a++)
				{
					choiceregions[a].On();
					choiceregions[a].SetRegionColor(greenColor);
				}

				gameTimer = SDL_GetTicks();
				latencyTimer = gameTimer;
				std::cerr << "Leaving IDLE state at " << SDL_GetTicks() << "." << std::endl;
				state = WaitDisp;
			}

			break;

		case WaitDisp:

			if( (CurTrial < NTRIALS) && (SDL_GetTicks()-gameTimer)>300)
			{
				Target.surebet = curtr.surebet;
				Target.gamble1 = curtr.gamble1;
				Target.gamble2 = curtr.gamble2;

				std::stringstream textc;
				//textc << "Recieve " << curtr.surebet << " cents.";
				textc << curtr.surebet << "\u00A2";
				delete choicetext[0];
				choicetext[0] = new Image(TTF_RenderText_Blended(choicefont, textc.str().c_str(), textColor));

				/*
				textc.str("");
				if (curtr.gamble1 < 0)
					textc << "Lose " << -curtr.gamble1 << " cents";
				else if (curtr.gamble1 == 0)
					textc << "Win 0 cents";
				else if (curtr.gamble1 < 100)
					textc << "Win " << curtr.gamble1 << " cents";
				else if (curtr.gamble1 == 100)
					textc << "Win 1 dollar";
				else //(curtr.gamble1 > 100)
					textc << "Win " << std::fixed << std::setprecision(2) << float(curtr.gamble1)/100.0f << " dollars";
				delete choicetext[1];
				choicetext[1] = new Image(TTF_RenderText_Blended(choicefont, textc.str().c_str(), textColor));
				
				std::stringstream textc2;
				if (curtr.gamble2 < 0)
					textc2 << "Lose " << -curtr.gamble2 << " cents";
				else if (curtr.gamble2 == 0)
					textc2 << "Win 0 cents";a
				else if (curtr.gamble2 < 100)
					textc2 << "Win " << curtr.gamble2 << " cents";
				else if (curtr.gamble2 == 100)
					textc2 << "Win 1 dollar";
				else //(curtr.gamble2 > 100)
					textc2 << "Win " << std::fixed << std::setprecision(2) << float(curtr.gamble2)/100.0f << " dollars";
				delete choicetext[3];
				choicetext[3] = new Image(TTF_RenderText_Blended(choicefont, textc2.str().c_str(), textColor));
				*/

				textc.str("");
				if (curtr.gamble1 < 0)
					textc << "Lose " << -curtr.gamble1 << "\u00A2";
				else if (curtr.gamble1 < 100)
					textc << curtr.gamble1 << "\u00A2";
				else if (curtr.gamble1 == 100)
					textc << "$1";
				else //(curtr.gamble1 > 100)
					textc << "$" << std::fixed << std::setprecision(2) << float(curtr.gamble1)/100.0f;
				delete choicetext[1];
				choicetext[1] = new Image(TTF_RenderText_Blended(choicefont, textc.str().c_str(), textColor));
				
				std::stringstream textc2;
				if (curtr.gamble2 < 0)
					textc2 << "Lose " << -curtr.gamble2 << "\u00A2";
				else if (curtr.gamble2 < 100)
					textc2 << curtr.gamble2 << "\u00A2";
				else if (curtr.gamble2 == 100)
					textc2 << "$1";
				else //(curtr.gamble2 > 100)
					textc2 << "$" << std::fixed << std::setprecision(2) << float(curtr.gamble2)/100.0f;
				delete choicetext[3];
				choicetext[3] = new Image(TTF_RenderText_Blended(choicefont, textc2.str().c_str(), textColor));

				for (a = 0; a < 4; a++)
					choicetext[a]->On();

				startbeep->Play();

				gameTimer = SDL_GetTicks();
				latencyTimer = gameTimer;
				std::cerr << "Leaving WAITDISP state at " << SDL_GetTicks() << "." << std::endl;
				state = ShowDisp;

			}

			break;


		case ShowDisp:
			//wait for subject response.

			//wait for participant to choose a gamble
			if (!endtrial && choiceinput > 0)
			{
				instructtext->Off();

				Target.choice = choiceinput;
				Target.lat = int(Target.time)-int(latencyTimer);
				hoverTimer = SDL_GetTicks();

				switch(choiceinput) {
					case 1:
						choiceregions[0].SetRegionColor(greenselColor);
						outcome = curtr.surebet;
						break;
					case 2:
						choiceregions[1].SetRegionColor(greenselColor);
						float flip = ((float) rand() / (RAND_MAX));
						outcome = (flip > 0.5 ? curtr.gamble1 : curtr.gamble2);
						break;
				}

				//we will update the score with the outcome of the gamble but will not report it.
				score += outcome;
				Target.outcome = outcome;
				Target.score = score;

				endtrial = true;
			}

			if (TRIALDUR > 0)  
				trialdur = TRIALDUR;
			else //if wait for completion of trial flag is set
				trialdur = WAITITI;


			if ((endtrial && (SDL_GetTicks()-hoverTimer > 500)) || (SDL_GetTicks()-gameTimer) > trialdur)
			{
				instructtext->Off();

				CurTrial++;
				gameTimer = SDL_GetTicks();
				
				manualwritedata = true;

				std::stringstream textss;
				textss << CurTrial + 1;
				delete trialnum;
				trialnum = new Image(TTF_RenderText_Blended(trialnumfont, textss.str().c_str(), textColor));

				std::cerr << "Trial " << CurTrial << " ended at " << SDL_GetTicks() << "." << std::endl;
				
				/*
				if (Target.choice == 1)
				{
					for (a = 1; a < 4; a++)
						choicetext[a]->Off();
				}
				else if (Target.choice == 2)
					choicetext[0]->Off();
				*/

				//if we have reached the end of the trial table, quit
				if (CurTrial >= NTRIALS)
				{
					std::cerr << "Leaving SHOWDISP state to FINISHED state." << std::endl;
					gameTimer = SDL_GetTicks();
					state = Finished;
				}
				else
					state = Idle;

			}

			break; //end ShowDisp state


		case Finished:
			// Trial table ended, wait for program to quit

			//BaseTrace->Off();
			
			for (a = 0; a < 2; a++)
				choiceregions[a].Off();

			for (a = 0; a < 4; a++)
				choicetext[a]->Off();

			choiceinput = 0;
			Target.choice = choiceinput;

			std::stringstream textd;
			if (score > 0)
				textd << "Based on all your gambles, you would have won $" << std::fixed << std::setprecision(2) << float(score)/100.0f << "!";
			else
				textd << "Based on all your gambles, you would have lost $" << std::fixed << std::setprecision(2) << float(-score)/100.0f << "!";

			delete text;
			text = new Image(TTF_RenderText_Blended(font, textd.str().c_str(), textColor));

			if ((SDL_GetTicks() - gameTimer) > 10000)
				quit = true;


			break;

			

	}
}

