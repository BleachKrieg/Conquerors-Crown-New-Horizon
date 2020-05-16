#ifndef __j1TUTORIAL_H__
#define __j1TUTORIAL_H__

#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"
#include "j1Timer.h"

struct SDL_Texture;
class j1Entity;
class DynamicEnt;
class StaticEnt;
class GuiItem;

enum Tutorial_states
{
	ST_Tutorial_Q0,
	ST_Tutorial_Q1,
	ST_Tutorial_Q2,
	ST_Tutorial_Q3,
	ST_Tutorial_Q4,
	ST_Tutorial_Q5,
	ST_Tutorial_Q6,
	ST_Tutorial_Q7,
	ST_Tutorial_Q8,
	ST_Tutorial_Q9,
	ST_Tutorial_Q9_1,
	ST_Tutorial_Q10,
	ST_Tutorial_Q10_1,
	ST_Tutorial_Q11,
	ST_Tutorial_Q11_1,
	ST_Tutorial_Q12,
	ST_Tutorial_Q13,
	ST_Tutorial_Q14,
	ST_Tutorial_Q15,
	ST_Tutorial_Q16,
	ST_Tutorial_Q17,
	ST_Tutorial_Finished
};

class j1Tutorial : public j1Module
{
public:

	j1Tutorial();

	// Destructor
	virtual ~j1Tutorial();

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

	void CheckTutorialStep(float dt);

	void GuiInput(GuiItem*);

	bool deleteUI(int step);

	void CreatePopUpMessage(int x, int y, char* titletext = "", char* text1 = "", char* text2 = "", char* text3 = "", char* text4 = "", char* text5 = "");
public:

	Tutorial_states ActualState;

	//Pop_up creator
	GuiItem* PopUpImage;
	GuiItem* PopUpText1;
	GuiItem* PopUpText2;
	GuiItem* PopUpText3;
	GuiItem* PopUpText4;
	GuiItem* PopUpText5;
	GuiItem* PopUpTitleText;
	GuiItem* PopUpButton;
	GuiItem* Uther_Image;

	// Step_1
	GuiItem* Question_1_text;
	GuiItem* Button_Yes;
	GuiItem* Button_Yes_Text;
	GuiItem* Button_No;
	GuiItem* Button_No_Text;

	// Arrows
	GuiItem* Arrow_1;
	GuiItem* Arrow_2;
	GuiItem* Arrow_3;
	GuiItem* Arrow_4;
	GuiItem* Arrow_5;
	GuiItem* Arrow_5_1;
	GuiItem* Arrow_6;
	GuiItem* Arrow_7;

	bool createUI;

	iPoint	 ingameUIPosition;

	// Minimap
	bool MinimapActive;

	// Camera
	bool moveCamera;
};

#endif // __j1TUTORIAL_H__