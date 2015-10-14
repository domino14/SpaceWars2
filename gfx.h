#ifndef GFX_H
#define GFX_H
#include <objbase.h>
#include <iostream.h>
#include <ddraw.h>
#include <dinput.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <fstream.h>
#include <mmsystem.h>

#define BOB_STATE_DEAD 0
#define BOB_STATE_ALIVE 1
#define BOB_STATE_LOADED 2
typedef struct BITMAP_FILE_TAG
        {
        BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
        BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
        PALETTEENTRY     palette[256];      // we will store the palette here
        UCHAR            *buffer;           // this is a pointer to the data

        } BITMAP_FILE, *BITMAP_FILE_PTR;

typedef struct BITMAP_OBJ_TYP
{
	int state;
	int attr;
	int x ,y;
	int xv, yv;
	int width, height;
	LPDIRECTDRAWSURFACE image;
} BITMAP_OBJ, *BITMAP_OBJ_PTR;

typedef struct STAR
{
	int layer; //layer 3 one pix per frame
	//layer 2 2 pix, layer 1 3 pix
	int xp;
	int yp;
	int color;
} star;

typedef struct BULLET
{
	int angle; /*90 degrees straight up.. 270 straight down
			   0 degrees right... 180 left..*/
	double velocity;
	double xp, yp;
	int colour;
	int life;
//	BITMAP_OBJ target; //x and y position of target
	int attr; //attributes.. may be a homing missile :)
} bullet;

typedef struct ENEMY
{
	 int hp;
	 int bulltype; // 0 normal 1 homing 2 aimed
	 int bullcount;
	 int xp, yp; 
	 int maxhp;
	 int bullvel;
	 int xv, yv;
	 int refresh; //refresh rate

	 int xload, yload; //x and y pos of cells to load
						//from
	 
	 int active; //0 no 1 yes
} enemy;

typedef struct POWERUP
{
	int active;
	int type; //1 laser 2 scatter 3 bomb 4 hp 5 double
	int xp, yp;
	int yv;
	int xload, yload;
} powerup;

typedef struct PLAYER
{
	int xp, yp;
	int energy;
	int curpow;
	int bombs;
	int kills;
	int score;
} player;
//trig table


int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename);
int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap);
int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height);
int Create_BOB(BITMAP_OBJ_PTR bob, int width, int height, int attr, int flags);
int Load_BOB(BITMAP_OBJ_PTR bob, BITMAP_FILE_PTR bitmap,
			 int cx, int cy, int mode);
int Draw_BOB(BITMAP_OBJ_PTR bob, LPDIRECTDRAWSURFACE dest);
int Destroy_BOB(BITMAP_OBJ_PTR bob);



#endif