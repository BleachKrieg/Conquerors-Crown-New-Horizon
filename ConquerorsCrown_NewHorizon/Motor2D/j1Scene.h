#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
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

	void LogoPushbacks();
	void LoadTiledEntities();

	void AddResource(char*, int);
	void TimeToClock();

	void GuiInput(GuiItem* guiElement);
	
private:
	bool changeEntities = false;
	scenes current_scene;
	p2SString logoSheet_file_name;
	SDL_Texture* logoSheet;
	SDL_Texture* victoryLogo;
	SDL_Texture* defeatLogo;
	Animation* current_animation = nullptr;
	Animation logo;
	int logoTextTimer;

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

	GuiItem* townHallButton;
	GuiItem* townHallImage;

	//LogoGui
	GuiItem* logoTextClick;
	GuiItem* logoBackground;

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

	j1Timer GameClock;
	int timer;
	string mins;
	string secs;

	bool active;
};

#endif // __j1SCENE_H__