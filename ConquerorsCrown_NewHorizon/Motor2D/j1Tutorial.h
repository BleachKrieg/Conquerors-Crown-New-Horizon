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
	ST_Tutorial_Q1,
	ST_Tutorial_Q2,
	ST_Tutorial_Q3,
	ST_Tutorial_Q4,
	ST_Tutorial_Q5,
	ST_Tutorial_Q6,
	ST_Tutorial_Q7,
	ST_Tutorial_Q8,
	ST_Tutorial_Q9,
	ST_Tutorial_Q10,
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
public:

	Tutorial_states ActualState;

	// Step_1
	GuiItem* Question_1_text;
	GuiItem* Button_Yes;
	GuiItem* Button_Yes_Text;
	GuiItem* Button_No;
	GuiItem* Button_No_Text;

	// Step_2
	GuiItem* Question_2_text;
	GuiItem* Question_2_text_2;
	GuiItem* Button_Next_2;
	GuiItem* Button_Next_2_Text;

	// Step_3
	GuiItem* Question_3_text;
	GuiItem* Question_3_text_2;
	GuiItem* Button_Next_3;
	GuiItem* Button_Next_3_Text;

	// Step_4
	GuiItem* Question_4_text;
	GuiItem* Question_4_text_2;

	// Step_5
	GuiItem* Question_5_text;
	GuiItem* Question_5_text_2;
	GuiItem* Button_Next_5;
	GuiItem* Button_Next_5_Text;

	// Step_6
	GuiItem* Question_6_text;
	GuiItem* Question_6_text_2;
	GuiItem* Button_Next_6;
	GuiItem* Button_Next_6_Text;

	// Step_7
	GuiItem* Question_7_text;
	GuiItem* Question_7_text_2;
	GuiItem* Button_Next_7;
	GuiItem* Button_Next_7_Text;

	// Step_8
	GuiItem* Question_8_text;
	GuiItem* Question_8_text_2;

	// Step_9
	GuiItem* Question_9_text;
	GuiItem* Question_9_text_2;
	GuiItem* Button_Next_9;
	GuiItem* Button_Next_9_Text;

	// Step_10
	GuiItem* Question_10_text;
	GuiItem* Question_10_text_2;
	GuiItem* Button_Next_10;
	GuiItem* Button_Next_10_Text;

	bool createUI;

	iPoint	 ingameUIPosition;

	// Minimap
	bool MinimapActive;

};

#endif // __j1TUTORIAL_H__