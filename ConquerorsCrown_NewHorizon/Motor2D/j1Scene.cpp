#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"


j1Scene::j1Scene() : j1Module()
{
	name.create("scene");


}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	pugi::xml_node map;

	for (map = config.child("map"); map; map = map.next_sibling("map")) {
		p2SString lvlname;
		lvlname.create(map.attribute("name").as_string());
		levels.add(lvlname.GetString());
	}
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	LOG("Start scene");

	/*current_level = "menu.tmx";
	
	if (App->map->Load(current_level.GetString()) == true)
	{
		App->audio->PlayMusic(App->map->data.music.GetString());

		LOG("%s", current_level.GetString());
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))

		RELEASE_ARRAY(data);
	}*/
	
	Hello = App->tex->Load("textures/Hello-World.png");
	//App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_1, 100, 200);
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate_Scene", Profiler::Color::Snow);


	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Update_Scene", Profiler::Color::Tomato);
		
	App->render->Blit(Hello, 0, 0, NULL, 1.0f, 1.0f);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		App->render->camera.y += 500*dt;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		App->render->camera.x += 500*dt;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		App->render->camera.y -= 500*dt;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		App->render->camera.x -= 500*dt;
	int x, y;
	App->input->GetMousePosition(x, y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
		App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_1, x, y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		App->entity->CreateEntity(DynamicEnt::DynamicEntityType::TEST_2, x, y-20);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		App->entity->CreateStaticEntity(StaticEnt::StaticEntType::TEST_3, x, y);

	
	//App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;
	
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->map->CleanUp();
	
	
	LOG("Freeing scene");

	return true;
}
bool j1Scene::Load(pugi::xml_node& data)
{
	LOG("Loading Scene state");
	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	LOG("Saving Scene state");
	

	pugi::xml_node scene = data.append_child("scenename");
	scene.append_attribute("name") = current_level.GetString();

	return true;
}

