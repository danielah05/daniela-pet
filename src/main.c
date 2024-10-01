#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h>

enum Status
{
	Idle,
	Walking,
	Sleeping
};

void meowanim(int *meowalpha, bool *domeow)
{
	*meowalpha -= 3;
	if (*meowalpha <= 0)
	{
		*domeow = false;
		*meowalpha = 255;
	}
}

void movetopoint(Vector2 *position, Vector2 *target, int *mw, int *mh, bool *move)
{
	if (*move == false)
	{
		target->x = GetRandomValue(0, *mw);
		target->y = GetRandomValue(0, *mh);
		*move = true;
	}
	else
	{
		*position = Vector2MoveTowards(*position, *target, 3.f);
	}
}

void eepysleepy(bool *eepy, int *eepychoose, int *status, int *eepytime, int *eepytick)
{
	if (*eepy == false)
	{
		*eepychoose = GetRandomValue(0, 200);
		if (*eepychoose >= 195)
		{
			*eepytime = GetRandomValue(5400, 16200); // 1:30 - 4:30 minutes
			*eepy = true;
		}
		else
			*status = Idle;
	}
	else
	{
		*eepytick += 1;
		if (*eepytick >= *eepytime)
		{
			*eepytick = 0;
			*eepy = false;
		}
	}
}

int main()
{
	// monitor width, height
	int mw = 0;
	int mh = 0;
	// window x, y + random
	int wx = 0;
	int wy = 0;
	int r_wx = 0;
	int r_wy = 0;
	// brain ticks
	int tick = 0;
	int tickhit = 0;
	// meow ticks
	int meowtick = 0;
	int meowtickhit = 0;
	// daniela status stuff
	int status = 0;
	bool move = false;
	bool eepy = false;
	int eepychoose = 0;
	int eepytime = 0;
	int eepytick = 0;
	int eepytext = 0;
	int eepytexttick = 0;
	// animation
    int animframes = 0;
    unsigned int nextframedataoffset = 0;
    int currentanimframe = 0;
    int framedelay = 1;
    int framecounter = 0;
	// meow animation
	int meowalpha = 255;
	bool domeow = false;

	// set up window flags
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_TOPMOST | FLAG_WINDOW_MOUSE_PASSTHROUGH);

	// init window and audio
	InitWindow(29, 38, "daniela-pet");
	InitAudioDevice();
	SetTargetFPS(60);

	// grab monitor size to position window in center
	mw = GetMonitorWidth(0);
	mh = GetMonitorHeight(0);
	wx = mw / 2;
	wy = mh / 2;

	// setup position vectors
	Vector2 position = (Vector2){wx, wy};
	Vector2 target = (Vector2){0, 0};

	// resource stuff
	SearchAndSetResourceDir("resources");
	Wave wavmeow = LoadWave("meow.wav");
	Sound sndmeow = LoadSoundFromWave(wavmeow);
	Image imsillywalk = LoadImageAnim("boogie.gif", &animframes);
	Texture2D texsillywalk = LoadTextureFromImage(imsillywalk);
	Image imsillysleep = LoadImage("sleep.png");

	// setup tick hit values
	tickhit = GetRandomValue(0, 60);
	meowtickhit = GetRandomValue(300, 3600);

	// main loop
	while (!WindowShouldClose())
	{
		// walk animation
		framecounter++;
        if (framecounter >= framedelay)
        {
            currentanimframe++;
            if (currentanimframe >= animframes) currentanimframe = 0;
            nextframedataoffset = imsillywalk.width*imsillywalk.height*4*currentanimframe;
            UpdateTexture(texsillywalk, ((unsigned char *)imsillywalk.data) + nextframedataoffset);
            framecounter = 0;
        }

		if (eepy == true)
			UpdateTexture(texsillywalk, ((unsigned char *)imsillysleep.data));

		// brain tick system
		if (status != Sleeping)
			tick++;
		if (tick >= tickhit)
		{
			if (status == Walking)
			{
				status = Idle;
				move = false;
			}
			else
			{
				status = GetRandomValue(0, 2);
			}
			tick = 0;
			tickhit = GetRandomValue(0, 600);
		}

		// daniela status
		switch (status)
		{
			case Idle:
				break;
			case Walking:
				movetopoint(&position, &target, &mw, &mh, &move);
				break;
			case Sleeping:
				eepysleepy(&eepy, &eepychoose, &status, &eepytime, &eepytick);
				break;
		}

		// meow tick system
		if (status != Sleeping)
			meowtick++;
		if (meowtick >= meowtickhit)
		{
			PlaySound(sndmeow);
			domeow = true;
			meowtick = 0;
			meowtickhit = GetRandomValue(300, 3600);
		}

		// meow animation
		if (domeow == true)
		{
			meowanim(&meowalpha, &domeow);
		}

		// set the window position
		SetWindowPosition(position.x, position.y);

		// drawing
		BeginDrawing();
		//ClearBackground(GRAY);
		ClearBackground(BLANK);
		DrawTexture(texsillywalk, 1, 9, WHITE);
		if (domeow == true)
			DrawText("MEOW", 0, 0, 10, (Color){255, 255, 255, meowalpha});
		if (eepy == true)
		{
			eepytexttick += 1;
			if (eepytexttick >= 60)
			{
				eepytext += 1;
				eepytexttick = 0;
			}

			if (eepytext == 1)
				DrawText("z", 16, 2, 10, (Color){255, 255, 255, 255});
			if (eepytext == 2)
			{
				DrawText("z", 16, 2, 10, (Color){255, 255, 255, 255});
				DrawText("z", 22, -3, 10, (Color){255, 255, 255, 255});
			}
			if (eepytext >= 3)
				eepytext = 0;
		}

		EndDrawing();
	}

	// cleanup
	UnloadTexture(texsillywalk);
	UnloadImage(imsillywalk);
	UnloadImage(imsillysleep);
	UnloadWave(wavmeow);
	UnloadSound(sndmeow);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
