#include "j1Minimap.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Tutorial.h"
#include "FoWManager.h"
#include "j1EntityManager.h"

j1Minimap::j1Minimap() : j1Module() {
	name.create("minimap");

	texture = nullptr;
	height = 100;
	width = 100;
	map_height = 200;
	scale = 1;
	margin = 0;
	corner = Corner::TOP_LEFT;
	minimap_test_rect = { 0,0,4,4 };
	visible = true;
	input = false;
}

j1Minimap::~j1Minimap() {
}

bool j1Minimap::Awake(pugi::xml_node& config) {
	uint window_width, window_height;

	//TODO 0: Take into account for the next TODO that the minimap widht is set in config
	width = config.attribute("width").as_int();

	//corner
	p2SString corner_string(config.attribute("corner").as_string());
	margin = config.attribute("margin").as_int();

	if (corner_string == "top_left") {
		corner = Corner::TOP_LEFT;
	}
	else if (corner_string == "top_right") {
		corner = Corner::TOP_RIGHT;
	}
	else if (corner_string == "bottom_left") {
		corner = Corner::BOTTOM_LEFT;
	}
	else if (corner_string == "bottom_right") {
		corner = Corner::BOTTOM_RIGHT;
	}

	return true;
}

bool j1Minimap::Start() {
	bool ret = true;
	uint window_width, window_height;
	App->win->GetWindowSize(window_width, window_height);

	//TODO 1: Calculate the dimensions of the minimap
	map_width = App->map->data.tile_width * App->map->data.width;
	map_height = App->map->data.tile_height * App->map->data.height;
	scale = ((width) / ((float)map_width));
	height = (map_height) * scale;

	minimap_entities = App->tex->Load("Assets/textures/units/Minimap/minimap_entities.png");
	//TODO 2: Create a texture for the minimap
	texture = SDL_CreateTexture(App->render->renderer, SDL_GetWindowPixelFormat(App->win->window), SDL_TEXTUREACCESS_TARGET,1.f * width, 1.f *height);
	
	//TODO 3: Set this texture as a rendering target and create the minimap
	LOG("%d",(SDL_SetRenderTarget(App->render->renderer, texture)));
	CreateMinimap();
	SDL_SetRenderTarget(App->render->renderer, nullptr);

	//calculate position depending on the chosen corner
	switch (corner)
	{
	case Corner::TOP_LEFT:
		position.x = margin;
		position.y = margin;
		break;
	case Corner::TOP_RIGHT:
		position.x = window_width - width - margin;
		position.y = margin;
		break;
	case Corner::BOTTOM_LEFT:
		position.x = margin;
		position.y = window_height - height - margin + 5;
		break;
	case Corner::BOTTOM_RIGHT:
		position.x = window_width - width - margin;
		position.y = window_height - height - margin;
		break;
	}

	return ret;
}
bool j1Minimap::PreUpdate(float dt) {
	bool ret = true;
	int mouse_x, mouse_y;

	if (input)		
	{
		//TODO 7: Move the camera when the player clicks on the minimap or scrolls the mouse on it while holding the left button
		if (((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) || (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)) && (App->scene->current_scene == scenes::ingame || App->scene->current_scene == scenes::tutorial && App->tutorial->MinimapActive == true))
		{
			App->input->GetMousePosition(mouse_x, mouse_y);
			SDL_Rect minimap = { App->minimap->position.x, App->minimap->position.y, App->minimap->width, App->minimap->height };

			if ((mouse_x > minimap.x) && (mouse_x < minimap.x + minimap.w) && (mouse_y > minimap.y) && (mouse_y < minimap.y + minimap.h))
			{
				//TODO 6: Use the function created in the previous TODO to transform the position of the mouse into Minimap coordinates and then to the world
				iPoint minimap_mouse_position;
				minimap_mouse_position = App->minimap->ScreenToMinimapToWorld(mouse_x, mouse_y);
				//LOG("Minimap position: x: %i y: %i", minimap_mouse_position.x, minimap_mouse_position.y);
				App->render->camera.x = -(minimap_mouse_position.x - App->render->camera.w * 0.5f);
				App->render->camera.y = -(minimap_mouse_position.y - App->render->camera.h * 0.5f);
			}
		}
	}
	

	return ret;
}
bool j1Minimap::Update(float dt) {

	//TODO 3: When you have the texture try blitting it on screen
	if (texture != NULL && visible)
	{
		App->render->Blit(texture, position.x, position.y, NULL, 0, 0);
	}
	
	//TODO 4.1: Fill the function WorldToMinimap to make the representation of the rect in the minimap be in the position it should 
	/*iPoint minimap_test_rect_position = App->minimap->WorldToMinimap(App->scene->test_rect.x, App->scene->test_rect.y);
	minimap_test_rect.x = minimap_test_rect_position.x;
	minimap_test_rect.y = minimap_test_rect_position.y;
	App->render->DrawQuad(minimap_test_rect, 255, 0, 0, 255,true,false);*/

	//TODO 4.2: Using WorldToMinimap create a white rect which represents the area that the camera records of the world onto the minimap 
	if (App->scene->current_scene == scenes::ingame || App->scene->current_scene == scenes::tutorial && App->tutorial->MinimapActive == true)
	{
		iPoint rect_position = WorldToMinimap(-App->render->camera.x, -App->render->camera.y);
		if (visible) 
		{
					
			for (int i = 0; i < App->entity->ai_dyn_ent.size(); i++) {
				SDL_Rect rect{ 2, 0, 2, 2 };
				iPoint minimap_rect_position = App->minimap->WorldToMinimap(App->entity->ai_dyn_ent[i]->position.x, App->entity->ai_dyn_ent[i]->position.y);
				
				App->render->Blit(minimap_entities, minimap_rect_position.x, minimap_rect_position.y, &rect, 0, 0);
			}
			for (int i = 0; i < App->entity->player_dyn_ent.size(); i++) {
				SDL_Rect rect{ 0, 0, 2, 2 };
				iPoint minimap_rect_position = App->minimap->WorldToMinimap(App->entity->player_dyn_ent[i]->position.x, App->entity->player_dyn_ent[i]->position.y);
				
				App->render->Blit(minimap_entities, minimap_rect_position.x, minimap_rect_position.y, &rect, 0, 0);
			}

			for (int i = 0; i < App->entity->player_stat_ent.size(); i++) {
				SDL_Rect rect{ 0, 2, 4, 4 };
				iPoint minimap_rect_position = App->minimap->WorldToMinimap(App->entity->player_stat_ent[i]->position.x, App->entity->player_stat_ent[i]->position.y);

				App->render->Blit(minimap_entities, minimap_rect_position.x, minimap_rect_position.y, &rect, 0, 0);
			}

			if (App->scene->current_scene == scenes::ingame && App->tutorial->MinimapActive == false && !App->scene->debug)
			{
				//Fog of war in minimap
				SDL_Rect section{ 0, 0, 225, 225 };
				App->render->Blit(App->fowManager->minimapFoWtexture, -App->render->camera.x + 15, -App->render->camera.y + 485, &section);
			}
			//Camera rectangle in minimap
			App->render->DrawQuad({ rect_position.x, rect_position.y, (int)(App->render->camera.w * scale),(int)(App->render->camera.h * scale) }, 255, 255, 255, 255, false, false);

		}
	}
	return true;
}

bool j1Minimap::PostUpdate(float dt) 
{

	return true;
}

bool j1Minimap::CleanUp() {
	App->tex->UnLoad(texture);
	App->tex->UnLoad(minimap_entities);
	texture = nullptr;
	input = false;
	return true;
}
bool j1Minimap::CreateMinimap() {

	PERF_START(ptimer);

	list<MapLayer*>::iterator layer_list;
	
	for (layer_list = App->map->data.layers.begin(); layer_list != App->map->data.layers.end(); ++layer_list) {
		MapLayer* layer = *layer_list;
		if (layer->returnPropValue("Nodraw") != 0)
			continue;

		int half_width = map_width * 0.5f;

		for (int y = 0; y < App->map->data.height; ++y)
		{
			for (int x = 0; x < App->map->data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = App->map->MapToWorld(x, y);
				//	pos = App->render->WorldToScreen(pos.x, pos.y);
					App->render->Blit(tileset->texture, pos.x, pos.y, &r, 1, 1, SDL_FLIP_NONE, scale);
				}
			}
		}
	}

	PERF_PEEK(ptimer);

	return true;
}

iPoint j1Minimap::WorldToMinimap(int x, int y) {
	//TODO 4.1: Fill this function
	iPoint minimap_position;
	minimap_position.x = position.x + x * scale;
	minimap_position.y = position.y + y * scale;

	return minimap_position;
}

iPoint j1Minimap::ScreenToMinimapToWorld(int x, int y) {
	//TODO 5: Fill this function to convert a position from screen to the Minimap and directly to world
	iPoint minimap_position;
	minimap_position.x = (x - position.x)/scale;
	minimap_position.y = (y - position.y)/scale;
	return minimap_position;
}