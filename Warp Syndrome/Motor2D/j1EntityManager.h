#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__
#include "j1App.h"
#include "j1Module.h"
#include "p2List.h"
#include "p2Defs.h"
#include "p2Point.h"
#include "p2SString.h"
#include "SDL/include/SDL.h"
#include "j1Textures.h"
#include "Entity.h"
#include "Player.h"
#include "j1Grenade.h"


//Manages all entities in the game
class j1EntityManager :public j1Module
{
public:
	j1EntityManager();
	~j1EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();
	bool Start(fPoint aGravity);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	Entity* CreateEntity(EntityType type);
	bool AddEntity(Entity*);
	bool DestroyEntity(Entity* entity);
	bool ClearEntitiesOfType(EntityType eType);
	bool RespawnEntitiesOfType(EntityType eType);
	bool SaveEntity(pugi::xml_node& ldata, Entity* enty) const;
	bool CleanAllEntites();
public:
	Player* player;
	Grenade* grenade;
	fPoint gravity;
	p2List<Entity*> entity_list;
	bool kill; // becomes true when an enemy kills the player
	
};



#endif // !__ENTITY_MANAGER_H__

