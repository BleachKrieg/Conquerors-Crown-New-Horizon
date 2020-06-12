#include "MouseCursor.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "J1GroupMov.h"
#include "j1Pathfinding.h"
#include "j1Render.h"
#include "j1Map.h"

MouseCursor::MouseCursor() : j1Module()
{
	name.create("mouse_cursor");
}

bool MouseCursor::Start() {

	// Control Variables
	on_resources = false;
	to_attack = false;

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
	iPoint tile;

	App->input->GetMousePosition(pos.x, pos.y);
	pos = App->render->ScreenToWorld(pos.x, pos.y);
	tile = App->map->WorldToMap(pos.x, pos.y);
	r = normal_cursor;

	if (App->input->mouse_on_screen && !App->scene->pauseMenu)
	{
		if (to_attack && App->movement->player_selected != nullptr)
			r = attack_cursor;
		if (on_resources && App->movement->player_selected != nullptr)
			r = resource_cursor;
		if (App->pathfinding->GetWalkability(tile) == 0 && App->movement->player_selected != nullptr)
			r = prohibited_cursor;
	}
	else
	{
		r = normal_cursor;
	}

	App->render->Blit(cursor_tex, pos.x, pos.y, &r );

	on_resources = false;
	to_attack = false;

	return true;
}

bool MouseCursor::CleanUp()
{
	App->tex->UnLoad(cursor_tex);

	return true;
}