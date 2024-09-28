#include "raylib.h"
#include "raymath.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#include <stdio.h>

enum Status
{
	Idle,
	Walking
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
	Sound sndmeow = LoadSound("meow.wav");
	Image imsillywalk = LoadImageAnim("boogie.gif", &animframes);
	Texture2D texsillywalk = LoadTextureFromImage(imsillywalk);

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

		// brain tick system
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
				status = GetRandomValue(0, 1);
			}
			tick = 0;
			tickhit = GetRandomValue(0, 600);
		}

		// daniela status (walk or idle)
		switch (status)
		{
			case Idle:
				break;
			case Walking:
				movetopoint(&position, &target, &mw, &mh, &move);
				break;
		}

		// meow tick system
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

		EndDrawing();
	}

	// cleanup
	UnloadTexture(texsillywalk);
	UnloadImage(imsillywalk);
	UnloadSound(sndmeow);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}
