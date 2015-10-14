#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#define SOUND_MIDI "sequencer"
#define SOUND_WAVE "waveaudio"
#define SOUND_DSBUFFER 2


class Sound
{

public:
	int Length;
	bool Initialized;

	char * Alias;
	
	void Create(char * alias, char * FileName, char * Type = SOUND_MIDI);
	void Play();
	void Stop();
	void Close();
	void Restart();
	int GetPosition();

	Sound();
	Sound(char * alias, char * FileName, char * Type = SOUND_MIDI);
	~Sound();
};



void InitDirectSound();
void EndDirectSound();
#endif