#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"


#define COORDS(a) a+3000 

struct SDL_Texture;
class j1Entity;
class DynamicEnt;
class StaticEnt;;

enum class scenes {
	menu,
	ingame,
	logo
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
	bool DeleteUI();

	//InGameUI functions
	bool CreateButtonsUI();
	bool DeleteButtonsUI();

	void LogoPushbacks();
	void LoadTiledEntities();
	void GuiInput(GuiItem* guiElement);
	
private:
	bool changeEntities = false;
	scenes current_scene = scenes::logo;
	p2SString logoSheet_file_name;
	SDL_Texture* logoSheet;
	Animation* current_animation = nullptr;
	Animation logo;
	int logoTextTimer;

public:
	p2SString current_level;
	//SDL_Texture* debug_tex;

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

	GuiItem* townHallButton;
	GuiItem* townHallImage;

	//LogoGui
	GuiItem* logoTextClick;
	GuiItem* logoBackground;

	bool active;
};

#endif // __j1SCENE_H__