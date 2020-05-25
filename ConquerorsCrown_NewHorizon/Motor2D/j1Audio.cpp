#include <math.h>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "DynamicEnt.h"
#include "StaticEnt.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.create("audio");
}

// Destructor
j1Audio::~j1Audio()
{}



bool j1Audio::Awake(pugi::xml_node & config)
{
	
	music_directory = config.child("music").child_value("folder");
	fx_directory = config.child("fx").child_value("folder");
	volumemusic = config.child("music").child("volumemusic").attribute("value").as_float();
	volumefx = config.child("fx").child("volumefx").attribute("value").as_float();
	

	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	construction = App->audio->LoadFx("Assets/Audio/SFX/Buildings/Construction_Loop_2.wav");
	cancel_building = App->audio->LoadFx("Assets/Audio/SFX/Buildings/Cancel_Building2.wav");
	
	go_footman = App->audio->LoadFx("Assets/Audio/SFX/Humans/footman/Footman_Move_2.wav");
	go_archer = App->audio->LoadFx("Assets/Audio/SFX/Humans/archer/Archer_Move_3.wav");
	go_gatherer = App->audio->LoadFx("Assets/Audio/SFX/Humans/Peasant/Peasant_Move.wav");
	footman_attack = App->audio->LoadFx("Assets/Audio/SFX/Combat/Metal_Light_Slice_Metal_1.wav");
	archer_attack = App->audio->LoadFx("Assets/Audio/SFX/Combat/Arrow_Throwing.wav");
	troll_attack = App->audio->LoadFx("Assets/Audio/SFX/Combat/Axe_Throwing.wav");
	grunt_attack = App->audio->LoadFx("Assets/Audio/SFX/Combat/Metal_Medium_Chop_Metal_3.wav"); 
	ogre_attack = App->audio->LoadFx("Assets/Audio/SFX/Combat/Metal_Heavy_Chop_Metal_3.wav");
	wood_gatherer = App->audio->LoadFx("Assets/Audio/SFX/Resources/Axe_Medium_Chop_Wood_4.wav");
	//mine_gatherer = App->audio->LoadFx("Audio/SFX/Humans/Peasant/Axe_Throwing.wav");
	die_footman = App->audio->LoadFx("Assets/Audio/SFX/Humans/footman/Footman_Death.wav");
	die_archer = App->audio->LoadFx("Assets/Audio/SFX/Humans/archer/Archer_Death2.wav");
	die_gatherer = App->audio->LoadFx("Assets/Audio/SFX/Humans/Peasant/Peasant_Warcry_1.wav");
	die_troll = App->audio->LoadFx("Assets/Audio/SFX/Orcs/Troll/Troll_Death2.wav");
	die_grunt = App->audio->LoadFx("Assets/Audio/SFX/Orcs/Grunt/Grunt_Death.wav");
	die_ogre = App->audio->LoadFx("Assets/Audio/SFX/Orcs/Ogre/Ogre_Death_1.wav");

	click_to_play = App->audio->LoadFx("Assets/Audio/SFX/UI/Big_Button_Click.wav");
	normal_click = App->audio->LoadFx("Assets/Audio/SFX/UI/Click.wav");
	pause_fx_in = App->audio->LoadFx("Assets/Audio/SFX/UI/Game_Pause_Fade_In.wav");
	pause_fx_out = App->audio->LoadFx("Assets/Audio/SFX/UI/Game_Pause_Fade_Out.wav");

	upgrade_complete = App->audio->LoadFx("Assets/Audio/SFX/Humans/Upgrade_Complete_1.wav");
	quest_complete = App->audio->LoadFx("Assets/Audio/SFX/UI/Good_Job.wav");
	quest_failed = App->audio->LoadFx("Assets/Audio/SFX/UI/Quest_Failed.wav");
	tutorial_complete = App->audio->LoadFx("Assets/Audio/SFX/UI/Quest_Completed.wav");
	
	logo_game_fx = App->audio->LoadFx("Assets/Audio/SFX/Logo/Logo_Game_SFX.wav");
	logo_team_fx = App->audio->LoadFx("Assets/Audio/SFX/Logo/Logo_Team_SFX.wav");
	
	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	for (int i = 0; i < fx.size(); i++) {
		Mix_FreeChunk(fx[i]);
	}

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

bool j1Audio::Update(float dt) {
	if (musicToFree && !Mix_PlayingMusic()) {
		Mix_FreeMusic(music);
		music = nullptr;
		musicToFree = false;
	}
	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		Mix_HaltMusic();

		Mix_FreeMusic(music);
	}

	p2SString tmp("%s%s", music_directory.GetString(), path);

	music = Mix_LoadMUS(tmp.GetString());

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());

		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());

				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play music %s. Mix_GetError(): %s", path, Mix_GetError());

				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

void j1Audio::PauseMusic(float fade_time)
{
	if (music != nullptr)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 900.0f));
			musicToFree = true;
		}
		else
		{
			Mix_HaltMusic();
		}
	}
}


// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());

	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool j1Audio::PlayFx(int channel, unsigned int id, int repeat)
{
	bool ret = false;

	if (!active)
		return false;

	if (id > 0 && id <= fx.size())
	{
		if (fx[id - 1] != nullptr) 
		{ 
			//FxVolume(-1, volumefx, sp_audio);
			Mix_PlayChannel(channel, fx[id - 1], repeat);
		}
		else {
			LOG("Could not play audio because there is no fx.");
		}

	}

	return ret;
}


void j1Audio::StopFx() {
	Mix_HaltChannel(-1);

}
bool j1Audio::channelFinished() {
	bool ret = false;
		return ret;
}

bool j1Audio::Save(pugi::xml_node& data) const
{
	pugi::xml_node music = data.append_child("volumemusic");
	pugi::xml_node fx = data.append_child("volumefx");
	music.append_attribute("value") = volumemusic;
	fx.append_attribute("value") = volumefx;
	return true;
}

bool j1Audio::Load(pugi::xml_node& data)
{
	volumemusic = data.child("volumemusic").attribute("value").as_float();
	volumefx = data.child("volumefx").attribute("value").as_float();
	return true;
}
void j1Audio::MusicVolume(float vol) {

	volumemusic = vol;

	if (volumemusic >= 1) {
		volumemusic = 1;
	}
	if (volumemusic <= 0) {
		volumemusic = 0;
	}

	Mix_VolumeMusic(128 * volumemusic);
}

void j1Audio::FxVolume(int channel, float vol) {
	
	volumefx = vol;
	
	if (volumefx >= 1) {
		volumefx = 1;
	}
	if (volumefx <= 0) {
		volumefx = 0;
	}
	Mix_Volume(channel, volumefx*128);
	//LOG("VOLUME SLIDER: %.2f", vol);
}

void j1Audio::SetChannelVolume(int channel, int volume)
{
	//volume_fx = volume_fx * spatial_audio_volume;
	Mix_Volume(channel, (volumefx * 128));
	LOG("VOLUME: %i", 128 - Mix_Volume(channel, -1));
}

