#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"


#define CURSOR_WIDTH 2

enum class Types
{
	image,
	text,
	button,
	inputText,
	slider,
};

struct _TTF_Font;
struct SDL_Color;

class GuiItem
{

public:
	GuiItem();
	virtual ~GuiItem();
	bool checkBoundaries(int, int);
	void Input();
	void SetFocus();
	void SetSingleFocus();

	void GetScreenRect(SDL_Rect&);
	SDL_Rect* GetLocalRect();
	void GetScreenPos(int&, int&);
	void GetLocalPos(int&, int&);
	void SetLocalPos(int&, int&);
	virtual void returnChilds(GuiItem*, GuiItem*) {
	}
	virtual void slide() {
	}
	virtual float returnSliderPos() {
		return 0;
	}
	virtual void SetText(const char*) {
	}
	
	virtual const char* GetText() const {
		return "none";
	}
	virtual GuiItem* GetInputText() const {
		return NULL;
	}
	virtual void setRects(SDL_Rect, SDL_Rect) {

	}

protected:
	int LocalX;
	int LocalY;
	SDL_Rect LocalRect;
	SDL_Rect idleRect;
	SDL_Rect illuminatedRect;
	SDL_Rect pushedRect;
	

public:
	int initposx;
	int initposy;
	GuiItem* parent;
	Types type;
	j1Module* CallBack;
	bool focus;
	SDL_Texture* texture;
	SDL_Rect textureRect;
	bool to_delete;
	bool isDynamic;
	bool follow;
	bool delayBlit;
};

class GuiImage: public GuiItem
{
public:
	GuiImage(int, int, SDL_Rect, j1Module* callback = nullptr);
	virtual ~GuiImage();

};

class GuiText : public GuiItem
{
public:
	GuiText(int, int, SDL_Rect, char*, j1Module* callback = nullptr);
	virtual ~GuiText();

private:
	SDL_Color color;
	const char *text;

public:
	const char* GetText() const;
	void SetText(const char*);
};

class GuiButton : public GuiItem
{
public:
	GuiButton(int, int, SDL_Rect, j1Module* callback = nullptr);
	virtual ~GuiButton();
	void setRects(SDL_Rect, SDL_Rect);
	
};

class InputText : public GuiItem
{
public:
	InputText(int x, int y, SDL_Rect texrect, j1Module* callback = nullptr);
	GuiItem* GetInputText() const;
	virtual ~InputText();
private:
	GuiItem* text;

	GuiItem* image;

};

class GuiSlider : public GuiItem
{
public:
	GuiSlider(int x, int y, SDL_Rect texrect, j1Module* callback = nullptr);
	virtual ~GuiSlider();
	void slide();
	float returnSliderPos();
	void returnChilds(GuiItem*, GuiItem*);
private:
	GuiItem* Image;
	GuiItem* ScrollThumb;

public:
	int dragarea;
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	//bool PreUpdate(float dt);

	// Called after all Updates
	bool Update(float dt);

	bool PreUpdate(float dt);

	bool PostUpdate(float dt);


	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods
	// Gui creation functions
	void IterateFocus();

	void DeleteGuiElement();

	void sendInput(GuiItem* item);

	SDL_Texture* GetAtlas() const;
	
	GuiItem* CreateGuiElement(Types type, int x, int y, SDL_Rect, GuiItem* parentnode = NULL, j1Module* callback = nullptr, char* text = "");

public:
	bool buttonPressed;
	int FocusIt;
	p2List<GuiItem*> guiElements;

private:
	p2SString atlas_file_name;
	SDL_Texture* atlas;

	int xblit, yblit;
public:
	bool debug;

};

#endif // __j1GUI_H__