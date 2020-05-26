#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"
#include "j1Timer.h"


#define COORDS(a) a+3000 

struct SDL_Texture;
class j1Entity;
class DynamicEnt;
class StaticEnt;;

enum class scenes {
	menu,
	tutorial,
	ingame,
	logo,
	victory,
	defeat
};

class GuiItem;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void UpdateCameraPosition(int speed);

	//Menu functions
	void DeleteScene();
	void CreateScene(scenes);

	bool CreateMenu();
	bool CreateTutorial();
	bool CreateInGame();

	bool CreateOptions();
	bool DeleteOptions();

	bool CreatePauseMenu();
	bool DeletePauseMenu();
	bool DeletePauseMenuButtons();

	bool CreateLogo();
	bool CreateVictory();
	bool CreateDefeat();
	bool DeleteUI();

	//InGameUI functions
	bool CreateButtonsUI();
	bool DeleteButtonsUI();

	void LogoPushbacks();
	void LoadTiledEntities();

	void AddResource(char*, int);
	void TimeToClock();

	void GuiInput(GuiItem* guiElement);
	

private:
	bool changeEntities = false;
	p2SString logoSheet_file_name;
	SDL_Texture* logoSheet;
	SDL_Texture* victoryLogo;
	SDL_Texture* defeatLogo;
	SDL_Texture* video_texture;
	SDL_Texture* videologo_tex;
	Animation* current_animation = nullptr;
	Animation* loader;
	Animation logo;
	Animation team_logo;
	int logoTextTimer;
	int logo_team_sfx_counter;
	int win_lose_counter;
	j1Timer logoTimer;
	int alpha;
	iPoint speed;
	bool wants_to_load = false;
	int time_loaded;
	
public:
	p2SString current_level;
	scenes current_scene;

	//SDL_Texture* debug_tex;

	uint gold;
	uint wood;
	uint stone;

	bool debug;
	bool Building_preview;
	iPoint mouse_position;
	iPoint map_coordinates;

	//MenuGui
	GuiItem* menuButtonNewGame;
	GuiItem* menuTextNewGame;
	GuiItem* menuButtonLoadGame;
	GuiItem* menuTextLoadGame;
	GuiItem* menuButtonOptions;
	GuiItem* menuTextOptions;
	GuiItem* menuButtonExit;
	GuiItem* menuTextExit;
	GuiItem* menuBackground;

	//InGameGui
	iPoint	 ingameUIPosition;
	GuiItem* ingameUI;
	GuiItem* ingameTopBar;
	GuiItem* ingameButtonMenu;
	GuiItem* ingameTextMenu;
	GuiItem* ingameTextGold;
	GuiItem* ingameTextWood;
	GuiItem* ingameTextStone;
	GuiItem* ingameTextClock;
	GuiItem* ingameTextWave;


	GuiItem* townHallButton;
	GuiItem* townHallImage;
	GuiItem* townHallWoodCostImage;
	GuiItem* townHallStoneCostImage;
	GuiItem* townHallWoodCostText;
	GuiItem* townHallStoneCostText;

	//OptionsGui
	bool optionsMenu;
	bool fullscreen;
	GuiItem* optionsBackground;
	GuiItem* optionsTitleText;
	GuiItem* optionsButtonClose;
	GuiItem* optionsTextClose;
	GuiItem* optionsMusicText;
	GuiItem* optionsMusicSlider;
	GuiItem* optionsFxText;
	GuiItem* optionsFxSlider;
	GuiItem* optionsButtonFullScreen;
	GuiItem* optionsTextFullScreen;

	//PauseMenuGui
	GuiItem* pausemenuBackground;
	GuiItem* pausemenuButtonResume;
	GuiItem* pausemenuTextResume;
	GuiItem* pausemenuButtonOptions;
	GuiItem* pausemenuTextOptions;
	GuiItem* pausemenuButtonSave;
	GuiItem* pausemenuTextSave;
	GuiItem* pausemenuButtonLoad;
	GuiItem* pausemenuTextLoad;
	GuiItem* pausemenuButtonExit;
	GuiItem* pausemenuTextExit;
	bool pauseMenu = false;

	//LogoGui
	GuiItem* logoTextClick;
	GuiItem* logoBackground;

	//TeamLogoGui
	SDL_Rect teamLogoBackground;

	//VictoryGui
	SDL_Rect rect_victory = { 0, 0, 757, 791 };
	GuiItem* victoryBackground;
	GuiItem* victoryButtonContinue;
	GuiItem* victoryTextContinue;
	GuiItem* victoryTextClick;
	float scale_victory = 0.0f;
	float speed_victory = 0.0f;

	//DefeatGui
	SDL_Rect rect_defeat = { 0, 0, 757, 791 };
	GuiItem* defeatBackground;
	GuiItem* defeatButtonContinue;
	GuiItem* defeatTextContinue;
	GuiItem* defeatTextClick;
	float scale_defeat = 0.0f;
	float speed_defeat = 0.0f;

	j1Timer gameClock;
	j1Timer Cooldown;

	int timer;
	string mins;
	string secs;
	bool finish = false;
	bool UiEnabled;
	bool loop = false;
	float last_dt;
	int intro_video;


	bool active;

	// Tutorial
	bool tutorial;
	j1Timer TutorialTimer;
	int camera_limit_x2;
	int camera_limit_y2;

	//Upgrade_Swordman
	bool Upgrade_Sowrdman;
	float stats_upgrade_swordman;
	bool Upgrade_Archer;
	float stats_upgrade_Archer;

	// Wall
	bool wall_create;

	int upgrade_swordman;
	int upgrade_archer;
	int upgrade_knight;
};

#endif // __j1SCENE_H__