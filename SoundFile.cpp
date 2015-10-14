#include "SoundFile.h"
#include <Stdio.h>
#include <windows.h>
#include <mmsystem.h>


//LPDIRECTSOUND lpds;

void Sound::Create(char * alias, char * FileName, char * Type)
{
Alias = alias;
	
char  Buffer[256];
char ReturnString[256];

//open the sound file
sprintf (Buffer, "open %s type %s alias %s",FileName, Type, Alias);
mciSendString (Buffer, NULL, NULL, NULL);

//set time mode to milliseconds
sprintf (Buffer, "set %s time format ms", Alias);
mciSendString (Buffer, NULL, NULL, NULL);

//get length of sound clip
//sprintf (Buffer, "status %s length", Alias);
//mciSendString (Buffer, ReturnString, 256, NULL);
//sscanf(ReturnString, "%d", Length);
Initialized = true;
}

void Sound::Play()
{
	if (!(Initialized)) 
		return;
	char  Buffer[200];
	sprintf(Buffer, "play %s", Alias);
	mciSendString (Buffer, NULL, NULL, NULL);
}

void Sound::Stop()
{
	if (!(Initialized)) 
		return;

	char Buffer[200];
	sprintf(Buffer, "stop %s", Alias);
	mciSendString (Buffer, NULL, NULL, NULL);
}

void Sound::Close()
{
	if (!(Initialized)) 
		return;

	char Buffer[200];
	sprintf(Buffer, "close %s", Alias);
	mciSendString (Buffer, NULL, NULL, NULL);
	Initialized = false;
}

void Sound::Restart()
{
	if (!(Initialized)) 
		return;

char Buffer[200];
sprintf (Buffer, "play %s from 0", Alias);
					
mciSendString (Buffer, NULL, NULL, NULL);
	
}

Sound::Sound(char * alias, char *FileName, char *Type)
{
Alias = alias;

char  Buffer[200];
sprintf (Buffer, "open %s type %s alias %s",FileName, Type, Alias);
mciSendString (Buffer, NULL, NULL, NULL);

}


int Sound::GetPosition()
{
	if (!(Initialized)) 
		return -1;

char  Buffer[256];
char ReturnString[256];
int Position;

//get length of sound clip
sprintf (Buffer, "status %s position", Alias);
mciSendString (Buffer, ReturnString, 256, NULL);
sscanf(ReturnString, "%d", &Position);

return Position;

}

Sound::~Sound()
{
	Close();
}

Sound::Sound()
{
Alias = NULL;
Initialized = false;

}

void InitDirectSound()
{


}

void EndDirectSound()
{

}
