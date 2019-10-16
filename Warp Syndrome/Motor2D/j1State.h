#ifndef __j1STATE_H__
#define __j1STATE_H__

#include "j1Module.h"
#include "p2List.h"

enum JumpingStatesX {
	JST_GOING_LEFT,
	JST_GOING_RIGHT,
	JST_IDLE
};

enum JumpingStatesY
{
	JST_GOING_UP,
	JST_TRANSITION,
	JST_GOING_DOWN,
	JST_UNKNOWN
};

enum state_list {
	NONE,
	IDLE,
	WALK_FORWARD,
	WALK_BACKWARD,
	RUN_FORWARD,
	RUN_BACKWARD,
	FREE_JUMP,
	FREE_FALLING,
	THROWING_GRENADE,
	THROWING_GRENADE_ON_AIR,
	TELEPORT,
	SLIDING_ON_RIGHT_WALL,
	SLIDING_ON_LEFT_WALL,
	WALL_JUMP,
	SLIDING_TO_IDLE,
	WAKE_UP,
	DEAD,
	GOD_MODE
};

class j1State : public j1Module
{
public:

	j1State();

	// Destructor
	virtual ~j1State();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Gives a state based on inputs
	void CheckInputs();
	// Checks Colliders
	void CheckColliders();
	// Moves player position
	void MovePlayer();
	// Puts the proper animation
	void ChangeAnimation();

	// Calculates jump shinanigans
	void JumpMoveX();
	void JumpMoveY();

	state_list current_state;

private:
	bool double_jump = true;
	bool grenade = false;
	int run_counter = 0;
	int internal_counter = 0;
	float jump_timer = 0;
	JumpingStatesX x_jumping_state = JST_IDLE;
	JumpingStatesY y_jumping_state = JST_UNKNOWN;
};

#endif // __j1STATE_H__