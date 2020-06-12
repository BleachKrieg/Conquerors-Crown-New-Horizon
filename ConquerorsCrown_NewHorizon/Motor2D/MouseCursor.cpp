#include "MouseCursor.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1Render.h"

MouseCursor::MouseCursor() : j1Module()
{
	name.create("mouse_cursor");
}

bool MouseCursor::Start() {

	// Control Variables
	on_resources = false;
	to_attack = false;
	is_blocked = false;

	// Cursor position in texture
	resource_cursor = { 0,0,32,32 };
	attack_cursor = { 32,0,32,32 };
	prohibited_cursor = { 0,32,32,32 };
	normal_cursor = { 32,32,32,32 };

	// Load cursor texture
	cursor_tex = App->tex->Load("Assets/textures/misc/cursors.png");

	return true;
}

bool MouseCursor::Update(float dt) {

	SDL_Rect r;
	iPoint pos;

	App->input->GetMousePosition(pos.x, pos.y);
	pos = App->render->ScreenToWorld(pos.x, pos.y);
	r = normal_cursor;

	if (App->input->mouse_on_screen && !App->scene->pauseMenu)
	{
		if (on_resources)
			r = resource_cursor;
		else if (is_blocked)
			r = prohibited_cursor;
		else if (to_attack)
			r = attack_cursor;
	}
	else
	{
		r = normal_cursor;
	}

	App->render->Blit(cursor_tex, pos.x, pos.y, &r );

	on_resources = false;
	is_blocked = false;
	to_attack = false;

	return true;
}

bool MouseCursor::CleanUp()
{
	App->tex->UnLoad(cursor_tex);

	return true;
}