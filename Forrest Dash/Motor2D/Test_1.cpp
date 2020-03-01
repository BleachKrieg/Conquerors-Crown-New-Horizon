#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "Animation.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Test_1.h"
#include "DynamicEnt.h"
#include "Brofiler/Brofiler.h"

Test_1::Test_1(int posx, int posy) : DynamicEnt(DynamicEntityType::TEST_1)
{
	name.create("test_1");

	actualState = ST_TEST_1_IDLE;
	speed = { 1, 1 };
	cost = 1;
	position.x = posx;
	position.y = posy;
	to_delete = false;
	// Load all animations
}

Test_1::~Test_1()
{}

bool Test_1::Start()
{

	return true;
}

bool Test_1::Update(float dt)
{
	BROFILER_CATEGORY("UpdateTest_1", Profiler::Color::BlanchedAlmond);

	CheckAnimation(dt);

	//App->render->Blit(App->entity->test_1_graphics, position.x + current_animation->pivotx[current_animation->returnCurrentFrame()], position.y + current_animation->pivoty[current_animation->returnCurrentFrame()], &(current_animation->GetCurrentFrame(dt)), 1.0f);
	App->render->DrawQuad({ position.x, position.y, 10, 10 }, 0, 200, 0);
	position.x += speed.x;
	if (position.x > 800)
		to_delete = true;
	return true;
}

bool Test_1::PostUpdate(float dt)
{
	BROFILER_CATEGORY("PostupdateTest_1", Profiler::Color::BurlyWood)

	return true;
}

void Test_1::CheckAnimation(float dt)
{

}