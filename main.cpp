/* Cesar Del Solar
some code from
Windows Game Programming for Dummies by Andre LaMothe
*/
// INCLUDES
#define WIN32_LEAN_AND_MEAN
#define INITGUID
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
#include "SoundFile.h"
#include "gfx.h"
#include <winbase.h>
#include <math.h>
//DEFINES
#define WINDOW_CLASS_NAME "WINCLASS1"
#define _RGB16BIT(r,g,b) ((b%32)+((g%32)<<5)+((r%32)<<10))
#define CKeyDown(x) keystate[x] & 0x80
const int SCREENWIDTH = 640;
const int SCREENHEIGHT = 480;
const double PI = 3.14159;

//GLOBALS
HWND main_window_handle = NULL;
HWND hwnd;
int gameover = 0;
DDSURFACEDESC ddsd;
DDSCAPS ddscaps;
LPDIRECTDRAWSURFACE lpddsprimary, lpddsback;
extern LPDIRECTDRAW lpdd;
LPDIRECTINPUT lpdi;
LPDIRECTINPUTDEVICE lpdikey;
LPDIRECTDRAWCLIPPER lpddclipper2;
UCHAR *video_buffer = NULL;
UCHAR keystate[256];
HINSTANCE myinst;
long delayer;
PALETTEENTRY palette[256];
LPDIRECTDRAWPALETTE lpddpal;
Sound MySound[20];
LPSYSTEMTIME systime;
BITMAP_FILE bitmap8bit;
HDC xdc;
star Star[300];
bullet Bullet[150];
enemy Enemy[10];
powerup Powerup[5];
BITMAP_OBJ ship;
double tcos[360];
double tsin[360];
player Player;

// FUNCTION PROTOTYPES//////////////
LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg,
							 WPARAM wparam, LPARAM lparam);
void DrawPixel(int x, int y, int palt);
void DrawBackPixel(int x, int y, int palt);
void DrawFillBox (int x1, int y1, int x2, int y2, int colour);
int Game_Init();
int Game_Shutdown();
int LoadPalette();
int Game_Main();
void LoadBMP(BITMAP_FILE_PTR bitmap, char *FileName);
RECT MakeRect (int x1, int y1, int x2, int y2);
void PrimSurfAllBlack();
void BackbufferManage(int flags);
LPDIRECTDRAWCLIPPER DD_Attach_Clipper(
	LPDIRECTDRAWSURFACE lpdds, int num_rects,
					 LPRECT clip_list);
void AnimateStars();
int BulletManage(int mode, bullet mybullet);
int AngleBet(int x1, int y1, int x2, int y2);
int GameOverScreen();
int EnemyManage(int mode, enemy MyEnemy);
void GameIntro();
int PowerupManage(int mode, powerup mypow);
void Powerupz(int x, int y);
//MAIN!////////////

int WINAPI WinMain(HINSTANCE hinstance, 
				   HINSTANCE hprevinstance, 
				   LPSTR lpcmdline, 
				   int ncmdshow)
{
//initialize window!
	WNDCLASS winclass;
	MSG msg;


	myinst = hinstance;
	winclass.style = CS_DBLCLKS | CS_OWNDC | 
					CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor (NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
//register window class
	srand((unsigned)time(NULL));
	if (!RegisterClass(&winclass))
		return (0);
//create window
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,
		"This window does nothing", WS_POPUP | WS_VISIBLE,
		0, 0, 100, 100, NULL, NULL, hinstance, NULL)))
		return (0);
	main_window_handle = hwnd;
	ShowCursor(0);
	int stderror = Game_Init ();

	//main event loop --- winx console.....
	while (gameover == 0)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			/*if (msg.message == WM_DESTROY)
				PostQuitMessage(0);*/
	

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} // end if

		// MAIN GAME PROCESSING GOES HERE
		Game_Main();

	} // end while

	//shut down
	Game_Shutdown();
	// return

	return(msg.wParam);
}
	
	
//THE FUNCTIONS..../////////////

LRESULT CALLBACK WindowProc (HWND hwnd, UINT msg,
							 WPARAM wparam, LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT ps;
	HDC			hdc;
	switch (msg)
	{
	case WM_CREATE: // called when window created
		{
			// init stuff
			return (0);
		}	break;

	case WM_PAINT: // called when window needs painting
		{
			// validate window
			hdc = BeginPaint (hwnd, &ps);
			EndPaint (hwnd, &ps);
			return (0);
		}	break;
	case WM_DESTROY: // called when window is killed
		
			// kill the application
			PostQuitMessage(0);
	
			return (0);
		 break;
	 
	default: break;
	} // end switch
	// process any messages that you didn't take care of
	return (DefWindowProc (hwnd, msg, wparam, lparam));
} // end WinProc


void DrawPixel(int x, int y, int palt)
{
	if (x < 0 || x > SCREENWIDTH - 1 || y < 0 || y > SCREENHEIGHT - 1) return;
	lpddsprimary->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	video_buffer = (UCHAR *)ddsd.lpSurface;
	video_buffer[x + y * ddsd.lPitch] = palt;
	lpddsprimary->Unlock(ddsd.lpSurface);
}
void DrawBackPixel(int x, int y, int palt)
{
if (x < 0 || x > SCREENWIDTH - 1 || y < 0 || y > SCREENHEIGHT - 1) return;
	lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	video_buffer = (UCHAR *)ddsd.lpSurface;
	video_buffer[x + y * ddsd.lPitch] = palt;
	lpddsback->Unlock(ddsd.lpSurface);
}
void DrawFillBox (int x1, int y1, int x2, int y2, int colour)
{
	/*Draws a filled box of color colour using opposite
	corners x1, y1 and x2, y2 */
	/*Because I'm lazy, x1 and y1 MUST be
	top left corner. But this is my code, so it doesn't
	matter */
	int a;
	int b;
	for (a = x1; a <= x2; a++)
	{
		for (b = y1; b <= y2; b++)
		{
			DrawPixel(a, b, colour);
		}
	}
}
int Game_Init()
{
	int a, b, x;
 /*Direct Draw stuff :P */
	if (DirectDrawCreate(NULL, &lpdd, NULL)!=DD_OK)
		return(0);
	lpdd->SetCooperativeLevel(hwnd, DDSCL_ALLOWMODEX |
		DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | 
		DDSCL_ALLOWREBOOT);
	if ((lpdd->SetDisplayMode(SCREENWIDTH,
		SCREENHEIGHT, 8))!=DD_OK)
		return(0);
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	if (lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)!=DD_OK)
		return(0);
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback);
	LoadPalette();
	
	/*clear screen to black*/
	for (a = 0; a < SCREENWIDTH; a++)
		for (b = 0; b < SCREENHEIGHT; b++)
			DrawPixel(a, b, 0); 
	/*Direct Input stuff :P */
		/*Keyboard*/
	if (DirectInputCreate(myinst, DIRECTINPUT_VERSION, &lpdi, NULL)!=DI_OK)
		return (0);
	if (lpdi->CreateDevice (GUID_SysKeyboard, &lpdikey, NULL)!=DI_OK)
		return (0);
	if (lpdikey->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)!=DI_OK)
		return (0);
	if (lpdikey->SetDataFormat (&c_dfDIKeyboard)!=DI_OK)
		return (0);
	if (lpdikey->Acquire()!=DI_OK)
		return (0);
	
/* Game stuff...*/
	/*initialize star field*/
		for (x = 0; x < 300; x++)
		{
			if (x < 100) Star[x].layer = 1;
			if (x >= 100 && x < 200) Star[x].layer = 2;
			if (x >= 200 && x < 300) Star[x].layer = 3;
			Star[x].xp = rand()% 640;
			Star[x].yp = rand()% 480;
			Star[x].color = 200;
			if (rand()%3 == 1) Star[x].color = 0;
			if (rand()%8 == 5) Star[x].color = rand()% 255; 
			
		}
		for (x = 0; x < 150; x++)
		{
			Bullet[x].angle = 0;
			Bullet[x].velocity = 0;
			Bullet[x].xp = 0;
			Bullet[x].yp = 0;
			Bullet[x].colour = 0;
		}
		for (x = 0; x < 5; x++)
		{
			Powerup[x].active = 0;
		}
		for (x = 0; x < 10; x++)
		{
			Enemy[x].active = 0;
		}
		for (x = 0; x < 360; x++)
		{
			tcos[x] = cos((double)x * (PI / 180));
			tsin[x] = sin((double)x * (PI / 180));
		}

	Player.bombs = 0;
	Player.kills = 0;
	Player.curpow = 0;
	return(1);
}

int Game_Shutdown ()
{
	if (lpddclipper2)
		lpddclipper2->Release();
	if (lpddpal)
		lpddpal->Release();
	if (lpddsprimary)
		lpddsprimary->Release();
	if (lpddsback)
		lpddsback->Release();

	if (lpdd!=NULL)
		lpdd->Release();
	
	lpdikey->Unacquire();
	lpdikey->Release ();
	lpdi->Release();
	//print out palette
	Unload_Bitmap_File(&bitmap8bit);
	return(1);
}

int LoadPalette()
{
	/*uses palette [] */
	ifstream i_f;
	int a;

	int x, y, z;
	i_f.open ("dfrmtcol.txt");
	for (a = 0; a < 256; a++)
	{
		i_f >> x;
		i_f >> y;
		i_f >> z;
		palette[a].peRed = (BYTE)x;
		palette[a].peGreen = (BYTE)y;
		palette[a].peBlue = (BYTE)z;
		palette[a].peFlags = PC_NOCOLLAPSE;
	}
	i_f.close();
	PALETTEENTRY colorr = {0, 0, 0, PC_NOCOLLAPSE};
	
	if ((lpdd->CreatePalette(DDPCAPS_8BIT | 
		DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL))!=DD_OK)
		return 0;
	lpddsprimary->SetPalette(lpddpal);
	//lpddpal->SetEntries(0, 0, 256, palette);
	return 1;
}



void PrimSurfAllBlack()
{
	memset(palette, 0, 256*sizeof(PALETTEENTRY));
	for (int index = 0; index < 256; index++)
		palette[index].peFlags = PC_NOCOLLAPSE;
	if (lpdd->CreatePalette(DDPCAPS_8BIT | 
		DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette,
		&lpddpal, NULL)!=DD_OK)
		return;
	if (lpddsprimary->SetPalette(lpddpal)!=DD_OK)
		return;
}

RECT MakeRect (int x1, int y1, int x2, int y2)
{
	/* Returns a rectangle with top left corner 
	x1, y1 and bottom right corner x2, y2 */
	RECT myrect;
	myrect.left = x1;
	myrect.top = y1;
	myrect.bottom = y2;
	myrect.right = x2;
	return myrect;
}

LPDIRECTDRAWCLIPPER DD_Attach_Clipper(
	LPDIRECTDRAWSURFACE lpdds, int num_rects,
					 LPRECT clip_list)
{
	int index;
	LPDIRECTDRAWCLIPPER lpddclipper;
	LPRGNDATA region_data;
	if ((lpdd->CreateClipper(0, &lpddclipper, NULL))!=DD_OK)
		return NULL;
	region_data = (LPRGNDATA) malloc (sizeof(RGNDATAHEADER) + num_rects * sizeof(RECT));
	memcpy(region_data->Buffer , clip_list, sizeof(RECT)*num_rects);
	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = num_rects * sizeof(RECT);

	region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.top = 64000;
	region_data->rdh.rcBound.right = -64000;
	region_data->rdh.rcBound.bottom = -64000;
	for (index = 0; index <num_rects; index++)
	{
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;
		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;
		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;
		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;
	}
	if ((lpddclipper->SetClipList (region_data, 0))!=DD_OK)
	{
		free (region_data);
		return NULL;
	}
	if ((lpdds->SetClipper (lpddclipper))!=DD_OK)
	{
		free (region_data);
		return NULL;
	}
	free (region_data);
	return (lpddclipper);
}

void BackbufferManage(int flags)
{
	UCHAR *video_buffer;
	int a, b;
	switch (flags)
	{
	case 0: //clear screen
		lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR 
	| DDLOCK_WAIT, NULL);
		//clear back screen
		video_buffer = (UCHAR *)ddsd.lpSurface;
		for (a = 0; a < 640; a++)
		{
			for (b = 0; b < 480; b++)
			{
				video_buffer[a + b * ddsd.lPitch] = 0;
			}
		}
		lpddsback->Unlock(ddsd.lpSurface);
	}
}

void AnimateStars()
{
	int a;


	for (a = 0; a < 300; a++)
	{
		if (Star[a].yp >= 480) Star[a].yp-= 480;
		DrawBackPixel (Star[a].xp, Star[a].yp, Star[a].color);
	
		switch (Star[a].layer)
		{
		case 1: Star[a].yp++;
		case 2: Star[a].yp++;
		case 3: Star[a].yp++;
		break;
		}
	}
}

int BulletManage(int mode, bullet mybullet)
{
	int a;
	int b = -100;

	DDBLTFX ddbltfx;
	RECT fill_area;
	switch(mode)
	{
	case 1: //Create new bullet
	 //scan for free bullets
		for (a = 149; a >= 0; a--)
		{
			if (Bullet[a].velocity == 0)
				b = a;
		}
		//spot b
		if (b == -100) return 0;
		Bullet[b] = mybullet;
		

	break;
	case 2: //animate existing bullets
		//x velocity = velocity cos angle
		//y velocity = velocity sin angle
		for (a = 0; a < 150; a++)
		{
		  if (!(Bullet[a].life == 0 && Bullet[a].velocity == 0))
		  {
			if (Bullet[a].attr == 1 && Bullet[a].velocity!= 0) //homing missile
			{
				//recompute new angle
				//based on targets - attr 1 
				//homes on player
				Bullet[a].angle = AngleBet
					(ship.x +32, ship.y+32,
					Bullet[a].xp, Bullet[a].yp);
			}
				if (Bullet[a].angle >= 360) Bullet[a].angle-= 360;
			if (Bullet[a].angle < 0) Bullet[a].angle +=360;
			if (Bullet[a].velocity != 0)
			{
				Bullet[a].xp+= (Bullet[a].velocity * tcos[Bullet[a].angle]);
				Bullet[a].yp+= (Bullet[a].velocity * -tsin[Bullet[a].angle]);
				Bullet[a].life++;
			}
			if (Bullet[a].life > 500 / Bullet[a].velocity) Bullet[a].xp = 700;
			if (Bullet[a].xp > 640 || Bullet[a].xp < 0
					|| Bullet[a].yp > 480 || Bullet[a].yp < 0)
					Bullet[a].velocity =
					Bullet[a].attr = Bullet[a].life = 0;
			 //effectively delete bullet
			
			memset(&ddbltfx, 0, sizeof(DDBLTFX));
			ddbltfx.dwSize = sizeof(DDBLTFX);
			ddbltfx.dwFillColor = Bullet[a].colour;
			fill_area.top = (int)Bullet[a].yp - 2;
			fill_area.left = (int)Bullet[a].xp - 2;
			fill_area.right = (int)Bullet[a].xp + 2;
			fill_area.bottom = (int)Bullet[a].yp + 2;
			lpddsback->Blt(&fill_area, NULL, NULL,
				DDBLT_COLORFILL | DDBLT_WAIT,
				&ddbltfx);
			//check if it hit anyone
			if (Bullet[a].xp > ship.x +16 && 
				Bullet[a].xp < ship.x + 48 &&
				Bullet[a].yp > ship.y + 16 &&
				Bullet[a].yp < ship.y + 48 &&
				Bullet[a].colour!=11)
				{

				Player.energy-=10;
				MySound[4].Restart();
				Player.curpow = 0;
				Bullet[a].velocity =
					Bullet[a].attr = Bullet[a].life = 0;
				}
				for (b = 0; b < 10; b++)
				if (Bullet[a].xp > Enemy[b].xp +16 && 
				Bullet[a].xp < Enemy[b].xp + 48 &&
				Bullet[a].yp > Enemy[b].yp + 16 &&
				Bullet[a].yp < Enemy[b].yp + 48 &&
				Bullet[a].colour == 11)
				Enemy[b].hp--;
				
			} //end if bullet active
		} // end for
		break;
	case 3:
		for (a = 0; a < 150; a++)
		{
			Bullet[a].velocity =
			Bullet[a].attr = Bullet[a].life = 0;
		}
		break;
	} // end switch
	return 1;

}

int AngleBet(int x1, int y1, int x2, int y2)
{
	//Returns angle between 0 and 359 
	//This is the angle that a unit at position
	//x2, y2, would need to aim to hit the target
	// at x1, y1
	int angle;
	if (sqrt(pow(x1-x2, 2)+pow(y1 - y2, 2)) == 0) return 0;
	angle = (int)(180/PI)*acos((x1 - x2)/(sqrt(pow(x1-x2, 2)+pow(y1 - y2, 2))));
			if (y1 > y2) angle = 360 - angle;
	return angle;
}

int EnemyManage(int mode, enemy MyEnemy)
{
	int a;
	int b = -100;
	BITMAP_OBJ tempbob;
	bullet mybull;
	switch (mode)
	{
	case 0: //create new enemy
		for (a = 9; a >= 0; a--)
		{
			if (Enemy[a].active == 0)
				b = a;
		}
		//spot b
		if (b == -100) return 0;
		Enemy[b] = MyEnemy;

		break;
	case 1: //enemy move and shoot
		for (a = 0; a < 10; a++)
		{
			if (Enemy[a].active == 1)
			{
				Enemy[a].yp+= Enemy[a].yv; //***
				Enemy[a].xp+=Enemy[a].xv;
				if (Enemy[a].xp > 580 
					|| Enemy[a].xp < 10) Enemy[a].xv = -Enemy[a].xv;
				
				Enemy[a].bullcount++;
				
				Create_BOB(&tempbob, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
				Load_BOB(&tempbob, &bitmap8bit, Enemy[a].xload, Enemy[a].yload, 0);
				tempbob.x = Enemy[a].xp;
				tempbob.y = Enemy[a].yp;
				Draw_BOB(&tempbob, lpddsback);
				Destroy_BOB(&tempbob);
				if (Enemy[a].bullcount > Enemy[a].refresh) 
				{
					mybull.attr = 0;
					if (Enemy[a].bulltype == 1) mybull.attr = 1;
					if (Enemy[a].bulltype == 2) mybull.angle = AngleBet(ship.x + 32,
						ship.y + 32, Enemy[a].xp + 32, Enemy[a].yp + 48);
					if (Enemy[a].bulltype == 0) mybull.angle = 270;
					mybull.velocity = Enemy[a].bullvel;
					mybull.colour = 201;
					mybull.life = 0;
					mybull.xp = Enemy[a].xp + 32;
					mybull.yp = Enemy[a].yp + 49;
						BulletManage(1, mybull);
					Enemy[a].bullcount = 0;
				}
				if (Enemy[a].yp > 480 || Enemy[a].xp > 640
					|| Enemy[a].xp < 0) Enemy[a].active = 0;
			
				if (Enemy[a].hp <= 0) 
				Enemy[a].active = 2;
				
			} //if enemy active
			if (Enemy[a].active == 2)
			{
				
				Create_BOB(&tempbob, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
				Load_BOB(&tempbob, &bitmap8bit, 1, 1, 0);
				tempbob.x = Enemy[a].xp;
				tempbob.y = Enemy[a].yp;
				Draw_BOB(&tempbob, lpddsback);
				Destroy_BOB(&tempbob);
				Enemy[a].active = 3;
				
			} else
			if (Enemy[a].active == 3)
			{
				
				Create_BOB(&tempbob, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
				Load_BOB(&tempbob, &bitmap8bit, 2, 1, 0);
				tempbob.x = Enemy[a].xp;
				tempbob.y = Enemy[a].yp;
				Draw_BOB(&tempbob, lpddsback);
				Destroy_BOB(&tempbob);
				Enemy[a].active = 4;
			} else
			if (Enemy[a].active == 4)
			{
				
				Create_BOB(&tempbob, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
				Load_BOB(&tempbob, &bitmap8bit, 1, 1, 0);
				tempbob.x = Enemy[a].xp;
				tempbob.y = Enemy[a].yp;
				Draw_BOB(&tempbob, lpddsback);
				Powerupz(Enemy[a].xp, Enemy[a].yp);
				Destroy_BOB(&tempbob);
				Enemy[a].active = 0;
				Player.score+=Enemy[a].maxhp * 10 + Enemy[a].bulltype * 10;
				Player.kills++;
			}
		} // for 
	break;
	case 3: //delete all enemies
		for (a = 0; a < 10; a++)
		{
			Enemy[a].active = 0;
		}
		break;

	} //switch
	
return 1;
}

int GameOverScreen()
{
	powerup mypow;
	enemy myenemy;
	bullet mybull;
	char buf[255];
	int flag, x;
	int a, b;
		for (a = 0; a < SCREENWIDTH; a++)
		for (b = 0; b < SCREENHEIGHT; b++)
			DrawPixel(a, b, 0); 
		lpddsprimary->GetDC (&xdc);
		    
		
			sprintf(buf, "You have achieved a score of %d.  \nWould you like to try again? Y/N ",Player.score);
			TextOut(xdc, 10, 320, buf, strlen(buf));
		lpddsprimary->ReleaseDC (xdc); 
		flag = 0;
		do{
			if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return 0;
			if (CKeyDown(DIK_N)) {
				gameover = 1;
				flag = 1;
				return 1;
			}
			if (CKeyDown(DIK_Y)) 
			{
				Player.score = 0;
				Player.energy = 100;
				flag = 1;
		
				for (x = 0; x < 10; x++)
				{
				Bullet[x].angle = 0;
				Bullet[x].velocity = 0;
				Bullet[x].xp = 0;
				Bullet[x].yp = 0;
				Bullet[x].colour = 0;
				}
				for (x = 0; x < 10; x++)
				{
				Enemy[x].active = 0;
				}
			
				MySound[0].Restart();
				Player.kills = 0;
				Player.bombs = 0;
				ship.x = 320;
				ship.y = 410;
				EnemyManage(3, myenemy);
				BulletManage(3, mybull);
				PowerupManage(3, mypow);
				return 0;
			}//if (CKeyDown(DIK_Y)) 
			
			
		} while (flag == 0);
		
}
			
void GameIntro()
{
	char buf[255];
	powerup mypow;
	int done = 0;
	int fcount = 0;
	int myrand;
	
;
	bullet mybull;
	enemy myenemy;
	DWORD xx;
	
	lpddsprimary->GetDC (&xdc);    
	sprintf(buf, "It is a time of communion within the United Federation of Planets..");
	TextOut(xdc, 10, 10, buf, strlen(buf));
	sprintf(buf, "No disturbances have been present for a long time.");
	
	TextOut(xdc, 10, 30, buf, strlen(buf));
	
//	AnimateStars();

	
	xx = timeGetTime();
	while (timeGetTime() < xx + 10000);
	sprintf(buf, "Until now...");
	
	TextOut(xdc, 50, 200, buf, strlen(buf));
	lpddsprimary->ReleaseDC(xdc);

	MySound[2].Play();

	lpddsprimary->GetDC(&xdc);
	xx = timeGetTime();
	while (timeGetTime() < xx + 4000);
	sprintf(buf, "An unknown fighter has raided the galaxy...");
	TextOut(xdc, 50, 300, buf, strlen (buf));
	xx = timeGetTime();
	while (timeGetTime() < xx + 4000);

lpddsprimary->ReleaseDC (xdc);
	while (done==0)
	{
		if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return;
	 if (timeGetTime() > xx + 10)
	 {
		 BackbufferManage(0);
		 AnimateStars();
		 fcount++;
		 ship.x+=rand()%6;
		 ship.y+= rand()%6;
		 Draw_BOB(&ship, lpddsback);
		 while (lpddsprimary->Flip (NULL, DDFLIP_WAIT)!=DD_OK);		
		 if (CKeyDown(DIK_ESCAPE)) goto theintro;
		 xx = timeGetTime();
		 
		 
	 }
	 if (fcount == 100) done = 1;
	}
	lpddsprimary->GetDC (&xdc);    
	sprintf(buf, "And the galaxy has turned to its space force for help..");
	TextOut(xdc, 10, 100, buf, strlen(buf));
	sprintf(buf, "Its three deadliest ship prototypes are finally being put to the test..");
	TextOut(xdc, 10, 120, buf, strlen(buf));
	lpddsprimary->ReleaseDC(xdc);
	xx = timeGetTime();
	while (timeGetTime() < xx + 6000);
	fcount = 0;
	done = 0;
	while (done==0)
	{
		if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return;
	 if (timeGetTime() > xx + 10)
	 {
		 BackbufferManage(0);
		 AnimateStars();
		 fcount++;
		 
		 Draw_BOB(&ship, lpddsback);
		 BulletManage(2, mybull);
		myrand = rand()%100;
		if (myrand >= 4 && myrand < 7)
		{
			myenemy.active = 1;
		
			myenemy.maxhp = 1;
			myenemy.hp = 1;
			if (myrand == 4)
			{
				myenemy.xload = 0;
			    myenemy.yload = 1;
				myenemy.bulltype = 0;
				myenemy.bullvel = 10;
				myenemy.bullcount = 19;
				myenemy.refresh = 18;
				myenemy.xv = 0;
				myenemy.yv = 7;
			}
			if (myrand == 5)
			{
				myenemy.xload = 2;
				myenemy.yload = 2;
				myenemy.bulltype = 1;
				myenemy.bullvel = 8;
					myenemy.refresh = 25;
					myenemy.bullcount = 26;
				myenemy.xv = 10;
				myenemy.yv = 7;
			}
			if (myrand == 6)
			{
				myenemy.xload = 1;
				myenemy.yload = 2;
				myenemy.bulltype = 2;
				myenemy.bullvel = 25;
					myenemy.refresh = 10;
					myenemy.bullcount = 11;
				
				myenemy.xv = -10;
				myenemy.yv = 7;
			}
			myenemy.xp = rand()%640;
			myenemy.yp = 0;
			EnemyManage(0, myenemy);
		}
		EnemyManage(1, myenemy);
			 if (fcount > 200 && fcount < 400)
			 {
			lpddsback->GetDC (&xdc);    
			sprintf(buf, "The intruder continued to take damage..");
			TextOut(xdc, 10, 400, buf, strlen(buf));
			sprintf(buf, "'But I was only here for peace!' he yelled agonically.");
			TextOut(xdc, 10, 420, buf, strlen(buf));
			lpddsback->ReleaseDC(xdc);
			 }
			 if (fcount > 500 && fcount < 700)
			 {
				lpddsback->GetDC (&xdc);    
				sprintf(buf, "The galaxy still felt that the strange intruder posed a threat to the peace..");
				TextOut(xdc, 10, 400, buf, strlen(buf));
				sprintf(buf, "They continued shooting, fearing another Great War..");
				TextOut(xdc, 10, 420, buf, strlen(buf));
				lpddsback->ReleaseDC(xdc);
			 }
			 if (fcount > 850)
			 {
				 lpddsback->GetDC (&xdc);  
				 sprintf(buf, "Unfortunately, the intruder got away with his life after his quick escape...");
				TextOut(xdc, 10, 400, buf, strlen(buf));
				lpddsback->ReleaseDC (xdc);  
			 }
		 while (lpddsprimary->Flip (NULL, DDFLIP_WAIT)!=DD_OK);		
			if (CKeyDown(DIK_ESCAPE)) goto theintro;
		 xx = timeGetTime();
		 
		 
	 }
	 if (fcount == 1000) done = 1;

	}
	////////////////////
	done = 0; fcount = 0;
	while (done==0)
	{
		if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return;
	 if (timeGetTime() > xx + 10)
	 {
		 BackbufferManage(0);
		 AnimateStars();
		 fcount++;
		 
		 Draw_BOB(&ship, lpddsback);
		 BulletManage(2, mybull);
		 ship.x-=rand()%4;
		 ship.y-=rand()%4;
		myrand = rand()%100;
		if (myrand >= 4 && myrand < 7)
		{
			myenemy.active = 1;
		
			myenemy.maxhp = 1;
			myenemy.hp = 1;
			if (myrand == 4)
			{
				myenemy.xload = 0;
			    myenemy.yload = 1;
				myenemy.bulltype = 0;
				myenemy.bullvel = 10;
				myenemy.bullcount = 19;
				myenemy.refresh = 18;
				myenemy.xv = 0;
				myenemy.yv = 7;
			}
			if (myrand == 5)
			{
				myenemy.xload = 2;
				myenemy.yload = 2;
				myenemy.bulltype = 1;
				myenemy.bullvel = 8;
					myenemy.refresh = 25;
					myenemy.bullcount = 26;
				myenemy.xv = 10;
				myenemy.yv = 7;
			}
			if (myrand == 6)
			{
				myenemy.xload = 1;
				myenemy.yload = 2;
				myenemy.bulltype = 2;
				myenemy.bullvel = 25;
					myenemy.refresh = 10;
					myenemy.bullcount = 11;
					myenemy.xv = -10;
				myenemy.yv = 7;
			}
			myenemy.xp = rand()%640;
			myenemy.yp = 0;
			EnemyManage(0, myenemy);
		}
		EnemyManage(1, myenemy);
			 
		 while (lpddsprimary->Flip (NULL, DDFLIP_WAIT)!=DD_OK);		
			if (CKeyDown(DIK_ESCAPE)) goto theintro;
		 xx = timeGetTime();
		 
		 
	 }
	if (fcount == 100) done =1 ;
	}
	EnemyManage(3, myenemy);
	BulletManage(3, mybull);
	PowerupManage(3, mypow);
	lpddsprimary->GetDC (&xdc);  
	sprintf(buf, "And it is now up to the fiend to wreak revenge on those who hurt him...");
	TextOut(xdc, 10, 300, buf, strlen(buf));
	lpddsprimary->ReleaseDC (xdc);  
xx = timeGetTime();
	while (timeGetTime() < xx + 5000);
theintro: //I swear this is my only label!!
	MySound[2].Close();
	MySound[3].Create("mysound", "title.mid", SOUND_MIDI);
	MySound[3].Play();
	done = 0;
	xx = timeGetTime();
	while (done == 0)
	{
		if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return;
	 if (timeGetTime() > xx + 10)
	 {
		 BackbufferManage(0);
		 AnimateStars();
		 
		lpddsback->GetDC (&xdc);    
	sprintf(buf, "SPACE WARS II: THE COMING OF THE SQUIRE");
	TextOut(xdc, 150, 100, buf, strlen(buf));
	
	sprintf(buf, "CesarWare (C)2000");
	TextOut(xdc, 170, 170, buf, strlen(buf));
	
	sprintf(buf, "Press TAB to start!!");
	TextOut(xdc, 300, 350, buf, strlen(buf));
	sprintf(buf, "Beta Version 1.0");
	TextOut(xdc, 10, 400, buf, strlen(buf));
	lpddsback->ReleaseDC(xdc);
	while (lpddsprimary->Flip (NULL, DDFLIP_WAIT)!=DD_OK);		

	if (CKeyDown(DIK_TAB)) done = 1;
	xx = timeGetTime();
	 }
	}			
	MySound[3].Stop();

}

int Game_Main()
{
	int bullcount;
	int a;
	powerup mypow;
	PALETTEENTRY colorr;
	bullet mybull;
	int myrand;
	UCHAR *primary_buffer = NULL;
	UCHAR *secondary_buffer = NULL;
	char x = ' ';
	DWORD xx;
	enemy myenemy;
	BITMAP_OBJ bull;
	int colcount = 0;
	int temp = 0;
char buf[255];

	

	MySound[0].Create("sc", "main.mid", SOUND_MIDI);

	MySound[1].Create("shoot", "shoot.wav", SOUND_WAVE);

	MySound[2].Create("intro", "intro.mid", SOUND_MIDI);
	MySound[4].Create("ow", "hurt.wav", SOUND_WAVE);
	MySound[5].Create("ohyeah", "ohyeah.wav", SOUND_WAVE);
	RECT cliplist[1] = { {0, 0, SCREENWIDTH, SCREENHEIGHT} };

	lpddclipper2 = DD_Attach_Clipper(lpddsback, 1, cliplist);

//	PrimSurfAllBlack();
	if (SCREENWIDTH == 640 && SCREENHEIGHT == 480)
	{
		
	 Load_Bitmap_File(&bitmap8bit, "hires64pix.bmp");

	 
	}
	lpddpal->SetEntries(0, 0, 256, bitmap8bit.palette);
	 colorr.peRed = colorr.peGreen = colorr.peBlue = 255;
	 colorr.peFlags = PC_NOCOLLAPSE; 
	lpddpal->SetEntries(0, 200, 1, &colorr);
	 colorr.peRed = colorr.peBlue = 0;
	 colorr.peGreen =255;
	 colorr.peFlags = PC_NOCOLLAPSE;
	lpddpal->SetEntries(0, 201, 1, &colorr);

	Create_BOB(&ship, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);

	Load_BOB(&ship, &bitmap8bit, 3, 0, 0);

//	Create_BOB(&bull, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
//	Load_BOB(&bull, &bitmap8bit, 0, 1, 0);

	GameIntro();	
		
MySound[0].Play();

	Player.energy = 100;
	
	

	ship.x = 320;
	ship.y = 410;
	Draw_BOB(&ship, lpddsprimary);

	

	xx = timeGetTime();
	BackbufferManage(0);
	bullcount = 0;
	mybull.angle = 0;
	
	while (x != 'q')   // MAIN LOOP
	{
	if (lpdikey->GetDeviceState(256, keystate)!=DI_OK)
		return 0;
				

	if (timeGetTime() > xx +  10)
	{	
		BackbufferManage(0);
		bullcount++;
		Draw_BOB(&ship, lpddsback);
		//Draw_BOB(&bull, lpddsback);
		AnimateStars();
		BulletManage(2, mybull);
		PowerupManage(2, mypow);
		myrand = rand()%100;
		if (myrand >= 4 && myrand < 7)
		{
			myenemy.active = 1;
		
			myenemy.maxhp = 1;
			myenemy.hp = 1;
			if (myrand == 4)
			{
				myenemy.xload = 0;
			    myenemy.yload = 1;
				myenemy.bulltype = 0;
				myenemy.bullvel = 13;
				myenemy.bullcount = 37;
				myenemy.refresh = 36;
					myenemy.xv = 0;
				myenemy.yv = 7;
			}
			if (myrand == 5)
			{
				myenemy.xload = 2;
				myenemy.yload = 2;
				myenemy.bulltype = 1;
				myenemy.bullvel = 8;
					myenemy.refresh = 50;
					myenemy.bullcount = 51;
					myenemy.xv = 10;
				myenemy.yv = 7;
			}
			if (myrand == 6)
			{
				myenemy.xload = 1;
				myenemy.yload = 2;
				myenemy.bulltype = 2;
				myenemy.bullvel = 25;
					myenemy.refresh = 30;
					myenemy.bullcount = 31;
					myenemy.xv = -10;
				myenemy.yv = 7;
			}
			myenemy.xp = rand()%580;
			myenemy.yp = 0;
			EnemyManage(0, myenemy);
		}
		EnemyManage(1, myenemy);
	lpddsback->GetDC (&xdc);
		    
		
			sprintf(buf, "Energy: %d    Score: %d   Bombs: %d  ", Player.energy, Player.score, Player.bombs);
			TextOut(xdc, 1, 1, buf, strlen(buf));
			sprintf(buf, "Kills: %d    ", Player.kills);
			TextOut(xdc, 560, 460, buf, strlen(buf));
		lpddsback->ReleaseDC (xdc); 
	//some logic 
		if (Player.energy <= 0) temp = GameOverScreen();
		if (temp == 1) x = 'q';
		while (lpddsprimary->Flip (NULL, DDFLIP_WAIT)!=DD_OK);		

		if (CKeyDown(DIK_RIGHT) && ship.x < SCREENWIDTH - 75) 
		{
			
		
			ship.x+=10;
					
		}
		if (CKeyDown(DIK_LEFT) && ship.x > 0) 
		{
			
		
			ship.x-=10;
			 	
		}
		if (CKeyDown(DIK_UP) && ship.y > 0) 
		{
			
			
			ship.y-=10;
			 
		}
		if (CKeyDown(DIK_DOWN) && ship.y < SCREENHEIGHT - 75) 
		{
		
			
			ship.y+=10;
			
		}
		if (CKeyDown(DIK_Q) || CKeyDown(DIK_ESCAPE)) x = 'q';
		if (CKeyDown(DIK_B))
		{
			//bomb
			if (Player.bombs > 0)
			{
				Player.bombs--;
				for (a = 0; a < 640; a+=10)
				{
				mybull.xp = a;
				mybull.yp = 480;
				mybull.angle = 90;
				mybull.velocity = 10;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				}
			}
		}

		if (CKeyDown(DIK_SPACE))
		{
			if (bullcount >= 5 )
			{
			if (Player.curpow !=5)
			{
				mybull.xp = ship.x + 32;
				mybull.yp = ship.y - 4;
				mybull.angle = 90;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				MySound[1].Restart();
				bullcount = 0;
			}
			if (Player.curpow == 5)
			{
				mybull.xp = ship.x + 48;
				mybull.yp = ship.y - 4;
				mybull.angle = 90;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				MySound[1].Restart();
				bullcount = 0;
				mybull.xp = ship.x + 16;
				mybull.yp = ship.y - 4;
				mybull.angle = 90;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
			}
				if (Player.curpow == 1)
				{
				mybull.xp = ship.x + 32;
				mybull.yp = ship.y - 8;
				mybull.angle = 90;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				}
				if (Player.curpow == 2)
				{
				mybull.xp = ship.x + 32;
				mybull.yp = ship.y - 4;
				mybull.angle = 75;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				mybull.xp = ship.x + 32;
				mybull.yp = ship.y - 4;
				mybull.angle = 105;
				mybull.velocity = 20;
				mybull.colour = 11;
				mybull.life = 0;
				mybull.attr = 0; //regular
				BulletManage(1, mybull); //create bullet
				}
				
			} // if bullcount > 5
		} // if space key pressed

			xx = timeGetTime();

	} // end timer if
		} // end while
	gameover = 1;
	MySound[0].Close();
	MySound[1].Close();
	MySound[2].Close();
	MySound[3].Close();
	MySound[4].Close();
	MySound[5].Close();
	Unload_Bitmap_File(&bitmap8bit);
	Destroy_BOB(&ship);
	
	return 0;
}

int PowerupManage(int mode, powerup mypow)
{
	int a;
	int b = -100;
	BITMAP_OBJ tempbob;
	switch (mode)
	{
	case 1: //create new powerup

	for (a = 4; a >= 0; a--)
		{
			if (Powerup[a].active == 0)
				b = a;
		}
		//spot b
		if (b == -100) return 0;
		Powerup[b] = mypow;
	break;
	case 2: //move powerups
		for (a = 0; a < 5; a++)
		{
			if (Powerup[a].active == 1)
			{
			Powerup[a].yp+=Powerup[a].yv;
			Create_BOB(&tempbob, 64, 64, 0, DDSCAPS_SYSTEMMEMORY);
			Load_BOB(&tempbob, &bitmap8bit, Powerup[a].xload, Powerup[a].yload, 0);
			tempbob.x = Powerup[a].xp;
			tempbob.y = Powerup[a].yp;
			Draw_BOB(&tempbob, lpddsback);
			Destroy_BOB(&tempbob);
			if (Powerup[a].yp > 480) Powerup[a].active = 0;
			//did player get powerup
			if (ship.x - 60 < Powerup[a].xp &&
				ship.x + 134 > Powerup[a].xp + 64 &&
				ship.y - 60 < Powerup[a].yp &&
				ship.y + 134 > Powerup[a].yp + 64)
				{
				Powerup[a].active = 0;
				MySound[5].Restart();
				if (Powerup[a].type == 3) Player.bombs++;
				if (Powerup[a].type == 4) Player.energy = 100;
				if (!(Powerup[a].type == 3 ||
					Powerup[a].type == 4))
					Player.curpow = Powerup[a].type; 
				}
			}
		}
		break;
	case 3:
		for (a = 0; a < 5; a++)
			Powerup[a].active = 0;
		break;
	}
	return 1;
}

void Powerupz(int x, int y)
{
	powerup mypow;
	
	if (rand()%4 != 3) return;
	mypow.active = 1;
	mypow.type = rand()%5 + 1;
	mypow.xload = mypow.type + 2;
//	mypow.xload = 3;
	mypow.yload = 3;
	mypow.yv = 7;
	mypow.xp = x;
	mypow.yp = y;
	
	
	PowerupManage(1, mypow);
}
