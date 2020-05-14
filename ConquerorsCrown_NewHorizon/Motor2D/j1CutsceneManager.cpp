#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Window.h"
#include "j1CutsceneManager.h"
#include "j1Minimap.h"


j1CutsceneManager::j1CutsceneManager() : j1Module()
{
	name.create("cutscene");
}

// Destructor
j1CutsceneManager::~j1CutsceneManager()
{}


// Called before the first frame
bool j1CutsceneManager::Start()
{
	black_bars.phase = None;
	black_bars.alpha = 0;
	int bar_height = 100;

	black_bars.top_rect.x = 0;
	black_bars.top_rect.y = 0;
	black_bars.top_rect.w = App->win->width;
	black_bars.top_rect.h = bar_height;

	black_bars.down_rect.x = 0;
	black_bars.down_rect.y = App->win->height - bar_height;
	black_bars.down_rect.w = App->win->width;
	black_bars.down_rect.h = bar_height;

	result = data.load_file("CutsceneEditor.xml");
	cutsceneManager = data.document_element();

	cinematic_camera.active = false;

	LOG("Starting Cutscene Manager");

	return true;
}

// Called each loop iteration
bool j1CutsceneManager::Update(float dt)
{
	if (cinematic_camera.active) 
	{
		DoCutscene(cinematic_camera, App->render->camera_pos);
	}

	return true;
}

// Called each loop iteration
bool j1CutsceneManager::PostUpdate(float dt)
{

	//Black Bars Drawing
	switch (black_bars.phase)
	{
	case FadeIn:
		black_bars.FadeIn();
		black_bars.Draw();
		break;
	case Drawing:
		black_bars.Draw();
		break;
	case FadeOut:
		black_bars.FadeOut();
		black_bars.Draw();
		break;
	}
	return true;
}

// Called before quitting
bool j1CutsceneManager::CleanUp()
{

	return true;
}

void BlackBars::FadeIn()
{
	//TODO 5.2: Complete this function doing a smooth fade in raising the value of the alpha 
	// (alpha = 0: invisible, alpha = 255: Completely black)
	alpha += 3;
	if (alpha >= 255) { alpha = 255;   phase = Drawing; }
}

void BlackBars::Draw()
{
	//TODO 5.1: Draw both quads unsing the alpha variable. Both rects are (top_rect and down_rect) already 
	// created, you just have to draw them.
	App->render->DrawQuad(top_rect, 0, 0, 0, alpha, true, false);
	App->render->DrawQuad(down_rect, 0, 0, 0, alpha, true, false);
}

void BlackBars::FadeOut()
{
	//TODO 5.2: Similar to fade in
	alpha -= 3;
	if (alpha == 0) { alpha = 0; phase = None; }
}

void j1CutsceneManager::StartCutscene(string name)
{
	if (!SomethingActive())
	{
		for (pugi::xml_node cutscene = cutsceneManager.child("cutscene"); cutscene; cutscene = cutscene.next_sibling("cutscene"))
		{
			if (cutscene.attribute("name").as_string() == name)
			{
				LoadSteps(cutscene);
			}
		}
		if (cinematic_camera.active) { cinematic_camera.UpdateStep(); }
		App->gui->SetGuiVisible(false);
		App->minimap->visible = false;
		App->scene->UiEnabled = false;
		App->minimap->input = false;
	}
}

bool j1CutsceneManager::LoadSteps(pugi::xml_node node)
{
	Step* s = new Step;

	for (pugi::xml_node step = node.child("step"); step; step = step.next_sibling("step"))
	{
		s->objective = step.attribute("objective").as_string();
		s->position.x = step.attribute("x").as_int();
		s->position.y = step.attribute("y").as_int();
		s->speed.x = step.attribute("vel_x").as_int();
		s->speed.y = step.attribute("vel_y").as_int();

		if (s->objective == "c")
		{
			cinematic_camera.steps.push_front(*s);
			cinematic_camera.active = true;
		}
	}
	if (black_bars.phase == None) { black_bars.phase = FadeIn; }
	delete s;

	return true;
}

void j1CutsceneManager::DoCutscene(CutsceneObject& object, iPoint& objective_position)
{
	LOG("DOING A CUTSCENE");
	Step step = object.current_step;

	if (object.active)
	{
		CameraMovement(step);

		if (step.position == object.steps.front().position)
		{
			FinishCutscene(object);
		}
		else if (step.position == objective_position)
		{
			object.UpdateStep();
		}
	}
}

void j1CutsceneManager::Movement(Step& step, iPoint& objective_position)
{
	if (step.position.x > objective_position.x)
	{
		if (step.position.x - objective_position.x > step.speed.x)
		{
			objective_position.x += step.speed.x;
		}
		else { objective_position.x++; }
	}

	else if (step.position.x < objective_position.x)
	{
		if (objective_position.x - step.position.x > step.speed.x)
		{
			objective_position.x -= step.speed.x;
		}
		else { objective_position.x--; }
	}

	if (step.position.y > objective_position.y)
	{
		if (step.position.y - objective_position.y > step.speed.y)
		{
			objective_position.y += step.speed.y;
		}
		else { objective_position.y++; }
	}

	else if (step.position.y < objective_position.y)
	{
		if (objective_position.y - step.position.y > step.speed.y)
		{
			objective_position.y -= step.speed.y;
		}
		else { objective_position.y--; }
	}
}

void j1CutsceneManager::CameraMovement(Step& step)
{
	if (step.position.x > App->render->camera.x)
	{
		if (step.position.x - App->render->camera.x > step.speed.x)
		{
			App->render->camera.x += step.speed.x;
		}
		else { App->render->camera.x++; }
	}

	else if (step.position.x < App->render->camera.x)
	{
		if (App->render->camera.x - step.position.x > step.speed.x)
		{
			App->render->camera.x -= step.speed.x;
		}
		else { App->render->camera.x--; }
	}

	if (step.position.y > App->render->camera.y)
	{
		if (step.position.y - App->render->camera.y > step.speed.y)
		{
			App->render->camera.y += step.speed.y;
		}
		else { App->render->camera.y++; }
	}

	else if (step.position.y < App->render->camera.y)
	{
		if (App->render->camera.y - step.position.y > step.speed.y)
		{
			App->render->camera.y -= step.speed.y;
		}
		else { App->render->camera.y--; }
	}
}


void CutsceneObject::UpdateStep()
{
	current_step.position.x = steps.back().position.x;
	current_step.position.y = steps.back().position.y;
	current_step.speed.x = steps.back().speed.x;
	current_step.speed.y = steps.back().speed.y;

	if (steps.size() > 0)
	{
		steps.pop_back();
	}
}

void j1CutsceneManager::FinishCutscene(CutsceneObject& object)
{
	object.steps.clear();
	object.active = false;
	if (black_bars.phase == Drawing) { black_bars.phase = FadeOut; }
	App->gui->SetGuiVisible(true);
	App->minimap->visible = true;
	App->scene->UiEnabled = true;
	App->minimap->input = true;

}

bool j1CutsceneManager::SomethingActive()
{
	bool ret;
	if (cinematic_camera.active) { ret = true; }
	else { ret = false; }
	return ret;
}