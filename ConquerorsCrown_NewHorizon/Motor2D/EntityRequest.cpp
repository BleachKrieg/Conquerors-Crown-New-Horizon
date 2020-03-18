#include "j1EntityManager.h"
#include "j1Entity.h"
#include "Test_1.h"
#include "Test_3.h"

#include "j1App.h"
#include<stdio.h>
#include "p2Log.h"
#include "j1Textures.h"
#include "Brofiler/Brofiler.h"
#include "EntityRequest.h"

EntityRequest::EntityRequest()
{
	name.create("entity request");
}

EntityRequest::~EntityRequest()
{}

bool EntityRequest::Awake(pugi::xml_node& config)
{

	return true;
}

bool EntityRequest::Start()
{


	return true;
}

bool EntityRequest::CleanUp()
{

	return true;
}

bool EntityRequest::Update(float dt)
{

	return true;
}

bool EntityRequest::PostUpdate(float dt)
{

	return true;
}

