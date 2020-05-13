#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "Brofiler/Brofiler.h"


j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");

	bool ret = true;

	//Get the path to the atlas file
	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	icons_file_name = conf.child("icons").attribute("file").as_string("");
	
	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	//Load the atlas file
	atlas = App->tex->Load(atlas_file_name.GetString());
	icons = App->tex->Load(icons_file_name.GetString());

	FocusIt = 0;
	//Turn debug off
	debug = false;
	return true;
}

// Update all guis
bool j1Gui::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Gui PreUpdate", Profiler::Color::LightGoldenRodYellow);
	
	bool ret = true;

	//Check if a button is pressed
	buttonPressed = false;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) 
	{
		buttonPressed = true;
	}

	/*if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) 
	{
		App->gui->IterateFocus();
	}*/

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		App->input->text.Cut(App->input->text.Length() - 1, App->input->text.Length()-1);
	}
	//Iterate all gui elements
	if (ret) 
	{
		for (int i = 0; i < guiElements.size(); i++)
		{
			int x, y;
			if (guiElements[i]->follow)
			{
				x = -App->render->camera.x + guiElements[i]->initposx;
				y = -App->render->camera.y + guiElements[i]->initposy;
				guiElements[i]->SetLocalPos(x, y);
			}
			//Get the mouse position
			App->input->GetMousePosition(x, y);
			iPoint temp = App->render->ScreenToWorld(x, y);
			x = temp.x;
			y = temp.y;
			if (guiElements[i]->isDynamic)
			{
				//Check if mouse is over a ui element and set the focus
				if (guiElements[i]->checkBoundaries(x, y))
				{
					guiElements[i]->SetFocus();
					ret = false;
				}
			}
		}
	}
	return true;
}

// Called after all Updates
bool j1Gui::Update(float dt)
{
	BROFILER_CATEGORY("Gui Update", Profiler::Color::LightGray);

	for (int i = 0; i < guiElements.size(); i++) {
		int x, y;
		if (guiElements[i]->focus)
			guiElements[i]->Input();

		guiElements[i]->GetScreenPos(x, y);
		if (!guiElements[i]->delayBlit)
		{
			if (guiElements[i]->type == Types::text)
			{
				App->render->Blit(guiElements[i]->texture, x, y, &guiElements[i]->textureRect);
			}
			else {
				App->render->Blit(GetAtlas(), x, y, &guiElements[i]->textureRect);
				//Check if debug mode is enabled
				if (debug)
				{
					SDL_Rect* rect = guiElements[i]->GetLocalRect();
					rect->x = x;
					rect->y = y;
					App->render->DrawQuad(*rect, 0, 0, 255, 100);
				}
			}
		}
	}

	

	return true;
}

bool j1Gui::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Gui PostUpdate", Profiler::Color::GreenYellow);

	for (int i = 0; i < guiElements.size(); i++) {
		if (guiElements[i]->type != Types::text && App->gui->buttonPressed == false) {
			guiElements[i]->focus = false;
		}
		int x, y;
		guiElements[i]->GetScreenPos(x, y);
		if (guiElements[i]->delayBlit)
		{
			if (guiElements[i]->type == Types::text)
			{
				App->render->Blit(guiElements[i]->texture, x, y, &guiElements[i]->textureRect);
			}
			else {
				App->render->Blit(GetAtlas(), x, y, &guiElements[i]->textureRect);
				//Check if debug mode is enabled
				if (debug)
				{
					SDL_Rect* rect = guiElements[i]->GetLocalRect();
					rect->x = x;
					rect->y = y;
					App->render->DrawQuad(*rect, 0, 0, 255, 100);
				}
			}
		}
	}

	DeleteGuiElement();

	return true;
}

// Called before quitting to clean all gui data and delete all elements
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	for (int i = 0; i < guiElements.size(); i++)
	{
		RELEASE(guiElements[i]);
	}
	guiElements.clear();

	return true;
}

//Delete all gui elements
void j1Gui::DeleteAllGui() {
	for (int i = 0; i < guiElements.size(); i++) 
	{
		guiElements[i]->to_delete = true;
	}
}

//Delete a gui element
void j1Gui::DeleteGuiElement() 
{
	for (int i = 0; i < guiElements.size(); i++)
	{
		if (guiElements[i] != nullptr) {
			if (guiElements[i]->parent != nullptr && guiElements[i]->parent->to_delete == true)
			{
				guiElements[i]->to_delete = true;
			}
		}
	}
	for (int i = 0; i < guiElements.size(); i++)
	{
		if (guiElements[i]->to_delete == true)
		{
				if (guiElements[i]->texture != nullptr) guiElements[i]->SetText("");
				guiElements.erase(guiElements.begin() + i);
				i--;
			
		}
	}
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::IterateFocus() 
{
	if (FocusIt == 0)
	{
		guiElements.at(guiElements.size()-1)->focus = false;
	}
	else
	{
		guiElements.at(FocusIt - 1)->focus = false;
	}
	
	if (FocusIt == guiElements.size())
		FocusIt = 0;

	guiElements.at(FocusIt)->focus = true;
	FocusIt++;
}



GuiItem::GuiItem() {

}

GuiItem::~GuiItem() {

}


GuiItem* j1Gui::CreateGuiElement(Types type, int x, int y, SDL_Rect rect, GuiItem* parentnode, j1Module* callback, char* text, _TTF_Font* font)
{
	GuiItem* ret;
	switch (type) 
	{
	case Types::image: ret = new GuiImage(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::text: ret = new GuiText(x, y, rect, text, font, callback); ret->parent = parentnode; break;
	case Types::button: ret = new GuiButton(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::inputText: ret = new InputText(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::slider: ret = new GuiSlider(x, y, rect, callback); ret->parent = parentnode; break;
	case Types::bar: ret = new GuiBar(x, y, rect, callback); ret->parent = parentnode; break;
	}

	guiElements.push_back(ret);

	return ret;
}

void j1Gui::sendInput(GuiItem* Item)
{
	Item->CallBack->GuiInput(Item);
}

//Check if something is inside the gui element
bool GuiItem::checkBoundaries(int x, int y) 
{
	if (type == Types::button)
		textureRect = idleRect; //idle Button

	int posx, posy;
	GetScreenPos(posx, posy);

	if (x > posx && x < (posx + LocalRect.w)) 
	{
		if (y > posy && y < (posy + LocalRect.h)) 
		{
			if (type == Types::button)
				textureRect = illuminatedRect; //Illuminated Button
			return true;
		}
	}
	return false;
}

//Set the focus to the gui element
void GuiItem::SetFocus() 
{
	if (type != Types::text) 
	{
		if (App->gui->buttonPressed == true)
		{
			for (int i = 0; i < App->gui->guiElements.size(); i++) {
				App->gui->guiElements[i]->focus = false;
				App->input->DisableTextInput();
			}
			focus = true;
		}
	}
	else {
		if (App->gui->buttonPressed == true)
		{ 
			for (int i = 0; i < App->gui->guiElements.size(); i++) {
				App->gui->guiElements[i]->focus = false;
				App->input->DisableTextInput();
			}
			App->input->EnableTextInput("");
			focus = true;
		}
	}
}

void GuiItem::SetSingleFocus() 
{
	if (type == Types::text)
	{
		for (int i = 0; i < App->gui->guiElements.size(); i++) 
		{
			App->gui->guiElements[i]->focus = false;
			App->input->DisableTextInput();
		}
		App->input->EnableTextInput("");
		focus = true;
	}
}

void GuiItem::Input() {

	if (type == Types::button) 
	{
		if (focus == true)
		{
			textureRect = pushedRect; //Pushed Button
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				iPoint temp;
				App->input->GetMousePosition(temp.x, temp.y);
				temp = App->render->ScreenToWorld(temp.x, temp.y);
				temp.x;
				temp.y;
				if (checkBoundaries(temp.x, temp.y))
					App->gui->sendInput(this);
			}
		}

	}
	if(parent != nullptr)
	{
		if (parent->type == Types::slider) {
			if (focus == true)
			{
				App->gui->sendInput(parent);
				parent->slide();
			}

		}

		if (parent->type == Types::inputText) {

			if (focus == true)
			{
				SDL_DestroyTexture(texture);
				if (App->input->text.GetString() != "") {
					texture = App->font->Print(App->input->text.GetString());
				}
				else texture = nullptr;
				App->font->CalcSize(App->input->text.GetString(), textureRect.w, textureRect.h);

				int x, y;
				GetScreenPos(x, y);
				App->render->DrawQuad({ x + textureRect.w, y, 1, textureRect.h }, 255, 255, 255);
				if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
					SetText(App->input->text.GetString());
					App->gui->sendInput(this);
					focus = false;
					App->input->DisableTextInput();
				}

			}
		}
	}
}

void GuiItem::GetScreenRect(SDL_Rect& rect) {
	GuiItem* it = this->parent;
	int addx = 0, addy = 0;
	while (it != NULL) {
		addx += it->LocalRect.x;
		addy += it->LocalRect.y;
		it = it->parent;
	}
	rect.x = addx + LocalRect.x;
	rect.y = addy + LocalRect.y;
	rect.w = LocalRect.w;
	rect.h = LocalRect.h;

}

SDL_Rect* GuiItem::GetLocalRect() {
	return 	&LocalRect;
}

void GuiItem::GetScreenPos(int& x, int& y) {
	GuiItem* it = this->parent;
	int addx = 0, addy = 0;
	while (it != NULL) {
		addx += it->LocalX;
		addy += it->LocalY;
		it = it->parent;
	}

	x = addx + LocalX;
	y = addy + LocalY;
}

void GuiItem::GetLocalPos(int& x, int& y) {
	x = LocalX;
	y = LocalY;
}

void GuiItem::SetLocalPos(int& x, int& y) {
	LocalX = x;
	LocalY = y;
}

void GuiItem::SetLocalSize(int& w, int& h) {
	LocalRect.w = w;
	LocalRect.h = h;
	textureRect.w = w;
	textureRect.h = h;
}

//-------------------------------------------------------------
GuiImage::GuiImage(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem() {
	type = Types::image;
	LocalX = initposx = x;
	LocalY = initposy = y;
	delayBlit = false;
	textureRect = texrect;
	LocalRect = textureRect;
	isDynamic = false;
	texture = App->gui->GetAtlas();
	focus = false;
	follow = false;
	CallBack = callback;
	to_delete = false;
}

GuiImage::~GuiImage() {

}

//-------------------------------------------------------------
GuiText::GuiText(int x, int y, SDL_Rect texrect,  char* inputtext, _TTF_Font* font, j1Module* callback) : GuiItem() {
	type = Types::text;
	text = inputtext;
	LocalX = initposx = x;
	LocalY = initposy = y;
	LocalRect = texrect;
	delayBlit = false;
	isDynamic = false;
	focus = false;
	follow = false;
	local_font = font;
	textureRect = texrect;
	color = SDL_Color{ 255,255,255 };
	CallBack = callback;
	texture = App->font->Print(text, color, local_font);
	App->font->CalcSize(text, textureRect.w, textureRect.h, local_font);
	to_delete = false;
}

GuiText::~GuiText() {

}

const char* GuiText::GetText() const
{
	return text;
}

void GuiText::SetText(const char* newtext)
{
	if (to_delete == false) {
		text = newtext;
		App->tex->UnLoad(texture);
		if (text != "") {
			texture = App->font->Print(text, color, local_font);
			App->font->CalcSize(text, textureRect.w, textureRect.h, local_font);
		}
		else 
		{
			texture = nullptr;
		}
	}
	else {
		App->tex->UnLoad(texture);
		texture = nullptr;
	}
}

//-------------------------------------------------------------
GuiButton::GuiButton(int x, int y, SDL_Rect idle_rect, j1Module* callback) : GuiItem() {
	type = Types::button;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = idle_rect;
	idleRect = idle_rect;
	LocalRect = textureRect;
	isDynamic = true;
	delayBlit = false;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	to_delete = false;
}

GuiButton::~GuiButton() {

}


void GuiButton::setRects(SDL_Rect iluminated, SDL_Rect pushed) {

	illuminatedRect = iluminated;
	pushedRect = pushed;
}
//--------------------------------------------------------------
InputText::InputText(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem()
	{
	type = Types::inputText;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = { 0, 0, 0, 0 };
	LocalRect = textureRect;
	isDynamic = true;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	delayBlit = false;

	to_delete = false;

	image = App->gui->CreateGuiElement(Types::image, 0, 0, { 444, 661, 244, 65 }, this);
	text = App->gui->CreateGuiElement(Types::text, 20, 15, texrect, this, callback, "Insert Text");
	text->isDynamic = true;
}

InputText::~InputText() {
	
}

GuiItem* InputText::GetInputText() const {
	return text;
}

//--------------------------------------------------------------
GuiSlider::GuiSlider(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem()
{
	type = Types::slider;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = { 0, 0, 0, 0 };
	LocalRect = textureRect;
	isDynamic = false;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	Image = App->gui->CreateGuiElement(Types::image, 0, 0, texrect, this);
	Image->delayBlit = false;
	ScrollThumb = App->gui->CreateGuiElement(Types::image, 0, -4, { 286, 157, 12, 17 }, this, callback);
	//ScrollThumb->setRects({ 78, 912, 46, 23 }, { 78, 888, 46, 23 });
	ScrollThumb->isDynamic = true;
	ScrollThumb->delayBlit = false;
	to_delete = false;
	delayBlit = false;
}

GuiSlider::~GuiSlider() {

}

void GuiSlider::slide()
{
	int x, y, LocalX, LocalY, ScreenX, ScreenY, parentx, parenty, difference, width;
	App->input->GetMousePosition(x, y);
	iPoint temp = App->render->ScreenToWorld(x, y);
	x = temp.x - 6;
	y = temp.y;


	width = Image->GetLocalRect()->w - ScrollThumb->GetLocalRect()->w + 1;

	GetScreenPos(parentx, parenty);

	ScrollThumb->GetScreenPos(ScreenX, ScreenY);
	ScrollThumb->GetLocalPos(LocalX, LocalY);

	if (x > ScreenX)
	{
		difference = LocalX + x - ScreenX;
		if (ScreenX <= parentx + Image->GetLocalRect()->w - ScrollThumb->GetLocalRect()->w)
		{

			ScrollThumb->SetLocalPos(difference, LocalY);
		}
		else
		{

			ScrollThumb->SetLocalPos(width, LocalY);
		}
	}
	else if (x < ScreenX)
	{
		difference = LocalX + x - ScreenX;
		if (ScreenX >= parentx)
		{
			ScrollThumb->SetLocalPos(difference, LocalY);
		}
		else
		{
			int pos = -1;
			ScrollThumb->SetLocalPos(pos, LocalY);
		}
	}
	else if (x == ScreenX)
	{
		ScrollThumb->SetLocalPos(LocalX, LocalY);
	}

}

float GuiSlider::returnSliderPos()
{
	int x, y;
	float a, b;
	a = Image->GetLocalRect()->w;
	b = ScrollThumb->GetLocalRect()->w;
	a = a - b;
	ScrollThumb->GetLocalPos(x, y);

	return x / a;
}

void GuiSlider::setSliderPos(float value)
{

	int x, y;
	float a, b;
	a = Image->GetLocalRect()->w;
	b = ScrollThumb->GetLocalRect()->w;
	a = a - b;
	ScrollThumb->GetLocalPos(x, y);
	x = value * a;
	ScrollThumb->SetLocalPos(x, y);

}

void GuiSlider::ReturnChilds(GuiItem * imagepointer, GuiItem * ScrollPointer)
{
	imagepointer = Image;
	ScrollPointer = ScrollThumb;
}
//--------------------------------------------------------------
GuiBar::GuiBar(int x, int y, SDL_Rect texrect, j1Module* callback) : GuiItem() {
	type = Types::bar;
	LocalX = initposx = x;
	LocalY = initposy = y;
	textureRect = { 0, 0, 0, 0 };
	LocalRect = textureRect;
	isDynamic = false;
	follow = false;
	texture = App->gui->GetAtlas();
	focus = false;
	CallBack = callback;
	originalSize = { texrect.x + 2, texrect.y + 2, texrect.w - 4, texrect.h - 4 };
	background = App->gui->CreateGuiElement(Types::image, 0, 0, texrect, this);
	background->delayBlit = false;
	fill = App->gui->CreateGuiElement(Types::image, 2, 2, { originalSize.x, originalSize.y + 9, originalSize.w, originalSize.h }, this);
	fill->delayBlit = false;
	to_delete = false;
	delayBlit = false;
	value = 0.0f;
	updateBar(value);
}
GuiBar::~GuiBar() {

}

void GuiBar::updateBar(float newValue) {
	value = newValue;
	int newWidth = (originalSize.w / 100.0f) * newValue;
	fill->SetLocalSize(newWidth, originalSize.h);

}

void GuiBar::ReturnChilds(GuiItem * bgPointer, GuiItem * fillPointer)
{
	bgPointer = background;
	fillPointer = fill;
}