#include <iostream> 
#include <sstream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Pathfinding.h"
#include "J1GroupMov.h"
#include "j1Minimap.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "EntityRequest.h"
#include "j1FadeToBlack.h"
#include "j1WaveSystem.h"
#include "j1CutsceneManager.h"
#include "j1Tutorial.h"
#include "FoWManager.h"
#include "j1Video.h"
#include "MouseCursor.h"
#include "AssetsManager.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	frames = 0;
	want_to_save = want_to_load = false;

	assetManager = new ModuleAssetsManager();
	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	minimap = new j1Minimap();
	fowManager = new FoWManager();
	entity = new j1EntityManager();
	requests = new EntityRequest();
	gui = new  j1Gui();
	font = new j1Fonts();
	pathfinding = new j1PathFinding();
	movement = new j1GroupMov();
	fade = new j1FadeToBlack();
	wave = new j1WaveSystem();
	cutscene = new j1CutsceneManager();
	tutorial = new j1Tutorial();
	video = new j1Video();
	mouse_cursor = new MouseCursor();


	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(assetManager);
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(tutorial);
	AddModule(scene);
	AddModule(requests);
	AddModule(movement);
	AddModule(entity);
	AddModule(pathfinding);
	AddModule(wave);
	AddModule(fowManager);
	AddModule(minimap);
	AddModule(gui);
	AddModule(mouse_cursor);
	AddModule(fade);
	AddModule(font);
	AddModule(cutscene);
	AddModule(video);

	// render last to swap buffer
	AddModule(render);

	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend())
	{
		delete* item;
		item++;
	}
	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);
	vsync.create("IDK");
	framecap.create("ON");
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	pause = false;
	bool ret = false;
	cap = true;

	list<j1Module*>::iterator item_list;
	item_list = modules.begin();
	j1Module* it = *item_list;
	it->Awake(config.child(it->name.GetString()));

	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());
		
		//Read from config file your framerate cap
		framerate = app_config.attribute("framerate_cap").as_int();
	}

	if(ret == true)
	{

		list<j1Module*>::iterator item_list;
		j1Module* it;

		for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
			
			if (item_list != modules.begin())
			{
				it = *item_list;
				ret = it->Awake(config.child(it->name.GetString()));
			}
		}

	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;

	list<j1Module*>::iterator item_list;
	j1Module* it;

	for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
		it = *item_list;
		ret = it->Start();
	}

	startup_time.Start();
	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true || quitGame)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;



	char* buffer;

	int bytesFile = App->assetManager->Load("config.xml", &buffer);

	// Loading document from memory with PUGI: https://pugixml.org/docs/manual.html#loading.memory
	pugi::xml_parse_result result = config_file.load_buffer(buffer, bytesFile);
	RELEASE_ARRAY(buffer);


	if (result == NULL) {
	LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	}
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	//Calculate the dt: differential time since last frame
	if (pause == false) { dt = frame_time.ReadSec(); }
		
	else if (pause == true){ dt = 0.0f; }
		
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true){
		LoadGameNow();
}
	// Framerate calculations --

	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;
	
	static char title[256];


	/*sprintf_s(title, 256, "Conqueror's Crown: New Horizon [Time since startup: %.3f FPS:%02u Av.FPS: %.2f Last Frame Ms: %02u Cap:%s Vsync: %s Tile:%d,%d Camera: %i %i width: %i]",
		seconds_since_startup, prev_last_sec_frame_count, avg_fps, last_frame_ms,framecap.GetString(), vsync.GetString(),
		App->scene->map_coordinates.x,App->scene->map_coordinates.y, App->render->camera.x, App->render->camera.y, App->render->camera.w);
	*/
	sprintf_s(title, 256, "Conqueror's Crown: New Horizon [FPS:%02u]", prev_last_sec_frame_count);

	App->win->SetTitle(title);

	delaytimer.Start();
	
	int delay = 1 * 1000 / framerate - last_frame_ms;
	
	if (delay > 0 && cap){
	SDL_Delay(1 * 1000 / framerate - last_frame_ms);
	}
//	LOG("We waited for %d milliseconds and got back in %f", (int)delaytimer.ReadMs(), delaytimer.ReadMs());
	//Measure accurately the amount of time it SDL_Delay actually waits compared to what was expected

}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;

	list<j1Module*>::iterator item_list;
	j1Module* it;
	j1Module* pModule = NULL;

	for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
		it = *item_list;
		
		pModule = it;

		if (pModule->active == false) {
			continue;
		}

		ret = it->PreUpdate(dt);
	}


	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;

	list<j1Module*>::iterator item_list;
	j1Module* it;
	j1Module* pModule = NULL;

	for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
		it = *item_list;

		pModule = it;

		if (pModule->active == false) {
			continue;
		}

		ret = it->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;


	list<j1Module*>::iterator item_list;
	j1Module* it;
	j1Module* pModule = NULL;

	for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
		it = *item_list;

		pModule = it;

		if (pModule->active == false) {
			continue;
		}

		ret = it->PostUpdate(dt);
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;
	list<j1Module*>::reverse_iterator item;
	item = modules.rbegin();

	while (item != modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}

	PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list	
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame() const
{
	
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?
	want_to_save = true;

}

// ---------------------------------------
void j1App::GetSaveGames(list<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::Pause()
{
	return pause = !pause;
}

bool j1App::GetPause()
{
	return pause;
}


bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;
	char* buffer;
	load_game.create("data/save_game.xml");

	int bytesFile = App->assetManager->Load(load_game.GetString(), &buffer);

	pugi::xml_parse_result result = data.load_buffer(buffer, bytesFile);
	RELEASE_ARRAY(buffer);

	//pugi::xml_document data;
	//pugi::xml_node root;
	//load_game.create("save_game.xml");
	//pugi::xml_parse_result result = data.load_file(load_game.GetString());

	
	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.GetString());
		root = data.child("game_state");

		list<j1Module*>::iterator item_list;
		j1Module* it = NULL;
		ret = true;
		for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
			it = *item_list;
			ret = it->Load(root.child(it->name.GetString()));
		}

		data.reset();
		if(ret == true)
		{
			LOG("...finished loading");
		}
		else
		{
			LOG("...loading process interrupted with error on module %s", (it != NULL) ? it->name.GetString() : "unknown");
		}
	}
	else {
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());
		}

	want_to_load = false;
	return ret;
}

bool j1App::CheckSaveGame()
{

	//pugi::xml_document data;
	//pugi::xml_node root;
	//load_game.create("save_game.xml");
	//pugi::xml_parse_result result = data.load_file(load_game.GetString());

	//if (result != NULL) {
	//	return true;
	//}
	//else {
	//	LOG("no available savegame");
	//	return false;
	//}

	pugi::xml_document data;
	pugi::xml_node root;
	char* buffer;
	load_game.create("data/save_game.xml");

	int bytesFile = App->assetManager->Load(load_game.GetString(), &buffer);

	pugi::xml_parse_result result = data.load_buffer(buffer, bytesFile);
	RELEASE_ARRAY(buffer);

	if (result == NULL)
		return false;
	return true;





}


bool j1App::SavegameNow()
{
	bool ret = true;
	save_game.create("save_game.xml");
	LOG("Saving Game State to %s...", save_game.GetString());
	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	list<j1Module*>::iterator item_list;
	j1Module* it = NULL;

	for (item_list = modules.begin(); item_list != modules.end() && ret == true; ++item_list) {
		it = *item_list;
		ret = it->Save(root.append_child(it->name.GetString()));
	}



	if (ret == true)
	{
		std::stringstream stream;
		data.save(stream);
		//stream.str().c_str(); //contingut del fitxer
		//stream.str().length(); //mida del fitxer
		App->assetManager->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());

		//data.save_file(save_game.GetString());
		LOG("... finished saving", save_game.GetString());
	
	}
	else {
	LOG("Save process halted from an error in module %s", (it != NULL) ? it->name.GetString() : "unknown");
	}
	data.reset();
	want_to_save = false;
	return ret;
}