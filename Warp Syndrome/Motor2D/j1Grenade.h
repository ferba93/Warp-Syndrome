#ifndef __j1GRENADE_H__
#define __j1GRENADE_H__

#include "j1Module.h"
#include "j1Collision.h"
#include "p2List.h"
#include "Entity.h"
#include "Particles.h"

enum grenade_states {
	GST_UNKNOWN,
	GST_MOVING_UP,
	GST_MOVING_DOWN,
	GST_MOVING_RIGHT_UP,
	GST_MOVING_RIGHT_DOWN,
	GST_MOVING_LEFT_UP,
	GST_MOVING_LEFT_DOWN,
	GST_EXPLODING,
	GST_UNUSABLE
};

class Grenade : public AnimatedParticle
{
public:

	Grenade();
	Grenade(fPoint position, fPoint speed, float health);

	// Destructor
	~Grenade();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Chechk collisions
	void GrenadeCollisions();

	void Teleport();

	// Moves the grenade outside of the colliders
	void CorrectCollider();

public:
	collision_type grenade_collider_buffer=NONE_COLLISION;
};

#endif // __j1GRENADE_H__