#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include <vector>

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Pause the music with or without fadeout
	void PauseMusic(float fade_time = DEFAULT_MUSIC_FADE_TIME);
	bool Update(float dt);
	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(int channel, unsigned int fx, int repeat);

	//Stop FX channel
	void StopFx();

	//Check if an fx stopped playing
	bool channelFinished();

	//Save module audio data
	bool j1Audio::Save(pugi::xml_node& config)const;

	//Load module audio data
	bool j1Audio::Load(pugi::xml_node& config);
	void musicvolume(float value);
	float fxvolume(float value);
	void SetChannelVolume(int channel, int volume);

public:

	int construction;
	int cancel_building;
	int select_footman;
	int select_archer;
	int select_gatherer;
	int go_footman;
	int go_archer;
	int go_gatherer;
	int footman_attack;
	int archer_attack;
	int troll_attack;
	int wood_gatherer;
	int mine_gatherer;
	int die_footman;
	int die_archer;
	int die_gatherer;
	int die_troll;

	int click_to_play;
	int normal_click;
	int upgrade_complete;

	int logo_game_fx;
	int logo_team_fx;

private:

	_Mix_Music* music = NULL;
	vector<Mix_Chunk*>	fx;
	p2SString			music_directory;
	p2SString			fx_directory;
	float volumemusic;
	float volumefx;

	bool musicToFree;
};

#endif // __j1AUDIO_H__