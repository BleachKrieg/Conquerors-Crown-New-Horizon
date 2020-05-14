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
	bool CreateInGame();
	bool CreateLogo();
	bool CreateVictory();
	bool CreateDefeat();
	bool DeleteUI();

	//InGameUI functions
	bool CreateButtonsUI();
	bool DeleteButtonsUI();
	void CreatePopUpMessage(int x, int y, char* titletext = "", char* text1 = "", char* text2 = "", char* text3 = "", char* text4 = "", char* text5 = "");

	void LogoPushbacks();
	void TeamLogoPushbacks();
	void LoadTiledEntities();

	void AddResource(char*, int);
	void TimeToClock();

	void GuiInput(GuiItem* guiElement);
	
private:
	bool changeEntities = false;
	scenes current_scene;
	p2SString logoSheet_file_name;
	p2SString teamLogoSheet_file_name;
	SDL_Texture* logoSheet;
	SDL_Texture* victoryLogo;
	SDL_Texture* defeatLogo;
	SDL_Texture* teamLogoSheet;
	Animation* current_animation = nullptr;
	Animation logo;
	Animation team_logo;
	int logoTextTimer;
	int logo_team_sfx_counter;
	j1Timer logoTimer;
	int alpha;
	iPoint speed;

public:
	p2SString current_level;
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
	GuiItem* PopUpImage;
	GuiItem* PopUpText1;
	GuiItem* PopUpText2;
	GuiItem* PopUpText3;
	GuiItem* PopUpText4; 
	GuiItem* PopUpText5;
	GuiItem* PopUpTitleText;
	GuiItem* PopUpButton;

	GuiItem* townHallButton;
	GuiItem* townHallImage;
	GuiItem* townHallWoodCostImage;
	GuiItem* townHallStoneCostImage;
	GuiItem* townHallWoodCostText;
	GuiItem* townHallStoneCostText;
	
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
	int timer;
	string mins;
	string secs;
	bool finish = false;
	bool UiEnabled;

	bool active;
};

#endif // __j1SCENE_H__