#include "level_manager.h"
#include "j1App.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "j1SceneManager.h"
#include "Player.h"



j1LevelManager::j1LevelManager()
{
	name.create("level_m");
}

j1LevelManager::~j1LevelManager()
{}

bool j1LevelManager::Awake(pugi::xml_node& conf)
{
	filename.create(conf.child("load").attribute("docname").as_string());
	return true;
}

bool j1LevelManager::Start()
{
	bool ret = true;
	//loads the player document
	pugi::xml_parse_result result = leveldoc.load_file(filename.GetString());

	if (result == NULL)
	{
		LOG("Could not load player documant. pugi error: %s", result.description());
		ret = false;
	}

	//initializes playernode to be the root node of the doc
	rootnode = leveldoc.child("level_list");
	LoadLevelList(rootnode);
	return true;
}

bool j1LevelManager::CleanUp()
{
	p2List_item<level*>* item;
	item = level_list.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	level_list.clear();
	return true;
}

//Save/Load
bool j1LevelManager::Load(pugi::xml_node& ldata)
{
	return true;
}
bool j1LevelManager::Save(pugi::xml_node& ldata) const
{
	return true;
}


bool j1LevelManager::RestartLevel()
{
	App->transitions->ChangeTransition(TM_RESTART_LEVEL, 2.0f);
	return true;
}
bool j1LevelManager::RestartGame()
{
	App->transitions->ChangeTransition(TM_RESTART_GAME, 2.0f);
	App->scene_manager->doingaction = true;
	return true;
}
bool j1LevelManager::ChangeToNextLevel()
{
	App->transitions->ChangeTransition(TM_CHANGE_TO_NEXT_LEVEL,1.0f);
	return true;
}
bool j1LevelManager::ChangeToLevel1()
{
	if (current_level != level_list.start)
	{
		App->transitions->ChangeTransition(TM_CHANGE_TO_NEXT_LEVEL,1.0f);
	}
	return true;
}
bool j1LevelManager::ChangeToLevel2()
{
	if (current_level != level_list.start->next)
	{
		App->transitions->ChangeTransition(TM_CHANGE_TO_NEXT_LEVEL,1.0f);
	}
	return true;
}


bool j1LevelManager::LoadLevelList(pugi::xml_node& root)
{

	//load level list
	pugi::xml_node leveliterator;
	for (leveliterator = root.child("level"); leveliterator; leveliterator = leveliterator.next_sibling("level"))
	{
		level* set = new level();

		set->hasnether = leveliterator.attribute("has_nether").as_bool();
		set->overworld = leveliterator.child("overworld").child_value();
		if (set->hasnether == true)
			set->nether = leveliterator.child("nether").child_value();
		else set->nether = NULL;

		level_list.add(set);
	}

	//load properties
	path = root.child("properties").attribute("path").as_string();
	numoflevels = root.child("properties").attribute("total_lvl_num").as_uint();

	p2List_item<level*>* defaultsearch = level_list.start;

	bool search = true;
	while (defaultsearch != NULL && search != false)
	{
		if (defaultsearch->data->overworld == root.child("properties").attribute("default").as_string())
		{
			default_level = defaultsearch;
			current_level = default_level;
			search = false;
		}
		defaultsearch->next;
	}
	return true;

}


bool j1LevelManager::RestartLevelObjects()
{
	App->entity_m->player->ResetPlayerToStart();
	App->entity_m->player->current_state = IDLE;
	App->entity_m->CleanAllEntites();
	App->entity_m->RespawnEntitiesOfType(EntityType::E_TYPE_ELEMENTAL);
	App->entity_m->RespawnEntitiesOfType(EntityType::E_TYPE_FIRE_SKULL);
	App->entity_m->RespawnEntitiesOfType(EntityType::E_TYPE_COIN_G);

	App->render->camera.x = -(App->entity_m->player->pos.x - (App->render->camera.w / 2));
	App->render->camera.y = -(App->entity_m->player->pos.y - (App->render->camera.h / 2));
	return true;
}

bool j1LevelManager::RestartGameObjects()
{
	current_level = level_list.start;
	App->map->ReloadMap(current_level->data->overworld.GetString());
	RestartLevelObjects();
	App->entity_m->player->ResetStates();
	
	return true;
}

bool j1LevelManager::Go_To_Next_Lvl()
{
	if (current_level->next == NULL)
	{
		current_level = level_list.start;
	}
	else current_level = current_level->next;

	App->map->ReloadMap(current_level->data->overworld.GetString());
	App->audio->PlayMusic(App->map->data.music_path.GetString());
	RestartLevelObjects();
	return true;
}