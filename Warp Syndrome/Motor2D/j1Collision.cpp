#include "p2Defs.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1Grenade.h"
#include "j1Render.h"
#include "p2List.h"
#include "p2SString.h"
#include "j1EntityManager.h"
#include "j1App.h"
#include "Player.h"

j1Collision::j1Collision() : j1Module() 
{
	name.create("collision");
}

j1Collision::~j1Collision() {}

bool j1Collision::Awake(pugi::xml_node& config) {
	//filename.create(config.child("load").attribute("docname").as_string());
	return true;
}

bool j1Collision::Start()
{
	current_collision = NONE_COLLISION;
	has_already_collided = false;
	return true;
}

bool j1Collision::Update(float dt) {

	bool ret = true;
	death_collider_touched = false;
	door_collider_touched = false;
	grenade_collider_touched = false;

	if(GroundCollision() == true){ y_player_buffer = App->entity_m->player->pos.y; }
	//player variables
	position.x = App->entity_m->player->pos.x - App->entity_m->player->hitbox_w_h.x / 2;
	position.y = App->entity_m->player->pos.y;
	measures.x = App->entity_m->player->hitbox_w_h.x;
	measures.y = App->entity_m->player->hitbox_w_h.y;
	if(App->entity_m->player->GetGodmode() == false){
		CheckLoop(&position, &measures, OBJECT_PLAYER);
	} else { current_collision = NONE_COLLISION; }
	y_player_buffer = App->entity_m->player->pos.y;

	return ret;
}

bool j1Collision::CleanUp() {
	//already cleaned in the map module
	return true;
}

void j1Collision::CheckLoop(fPoint *position, fPoint *measures, object_colliding object_name) {
	
	p2List_item<ObjectGroup*>* itemOG = pObjGroupList.start;
	//Reset bools
	has_already_collided = false;
	current_collision_buffer.is_first_collider_horizontal = false;

	// This array will check which collisions are happening, and change the value from NONE_COLLISION to the actual one
	//iterates all the object layers, and then iterates between all the object each layer contains
	collision_array[0] = NONE_COLLISION;
	collision_array[1] = NONE_COLLISION;
	collision_array[2] = NONE_COLLISION;
	collision_array[3] = NONE_COLLISION;
	collision_array[4] = LEFT_UPPER_COLLISION;
	collision_array[5] = RIGHT_UPPER_COLLISION;
	collision_array[6] = NONE_COLLISION;
	collision_array[7] = LEFT_GROUND_COLLISION;
	collision_array[8] = RIGHT_GROUND_COLLISION;
	collision_array[9] = GROUND_UPPER_COLLISION;

	while (itemOG != NULL)
	{
		p2List_item<Object*>* itemO = itemOG->data->objlist.start;
		while (itemO != NULL)
		{
			current_collider_type = GetCurrentCollider(itemO->data->type);
			switch (current_collider_type) {
			case death_collider:
			case regular_collider:
				current_collision = CheckCollision(itemO, &position->x, &position->y, &measures->x, &measures->y);
				collision_array[current_collision] = current_collision;
				if (current_collider_type == death_collider && current_collision != NONE_COLLISION) {
					death_collider_touched = true;
				}
				break;
			case grenade_collider:
				if (object_name != OBJECT_GRENADE) {
					current_collision = CheckCollision(itemO, &position->x, &position->y, &measures->x, &measures->y);
					collision_array[current_collision] = current_collision;
				}
				else{
					if (grenade_collider_touched == false) {
						grenade_collider_touched = InsideCollider(itemO, &position->x, &position->y, &measures->x, &measures->y);
					}
				}
				break;
			case door_collider:
				if (object_name == OBJECT_PLAYER) {
					current_collision = CheckCollision(itemO, &position->x, &position->y, &measures->x, &measures->y);
					collision_array[current_collision] = current_collision;
					if (current_collision != NONE_COLLISION) { door_collider_touched = true; }
				}
				break;
			case under_platform_collider:
				if (y_player_buffer <= itemO->data->boundingbox.y) {
					current_collision = CheckCollision(itemO, &position->x, &position->y, &measures->x, &measures->y);
					collision_array[current_collision] = current_collision;
				}
				break;
			default:
				break;
			}

			itemO = itemO->next;
		}


		itemOG = itemOG->next;
	}
	current_collision = CalculateFinalCollision(collision_array, current_collision);
}

collision_type j1Collision::CheckCollision(p2List_item<Object*>* currentobj, float *x, float *y, float *w,  float *h) {

	//collider variables
	float collider_x = (float)currentobj->data->boundingbox.x;
	float collider_y = (float)currentobj->data->boundingbox.y;
	float collider_w = (float)currentobj->data->boundingbox.w;
	float collider_h = (float)currentobj->data->boundingbox.h;

	if (collider_y <= *y && collider_y > *y - *h && collider_x < *x + *w && collider_x + collider_w > *x) {
		CollisionPriority(collider_x, collider_y, true);
		return GROUND_COLLISION;
	}
	else if (collider_x > *x && collider_x <= *x + *w && collider_y < *y && collider_y + collider_h > *y - *h) {
		CollisionPriority(collider_x - *w, collider_y, false);
		return RIGHT_COLLISION;
	}
	else if (collider_x + collider_w < *x + *w && collider_x + collider_w >= *x && collider_y < *y && collider_y + collider_h > *y - *h) {
		CollisionPriority(collider_x + collider_w, collider_y, false);
		return LEFT_COLLISION;
	}
	else if (collider_y + collider_h >= *y - *h && collider_y + collider_h <= *y && collider_x < *x + *w && collider_x + collider_w > *x) {
		CollisionPriority(collider_x, collider_y + collider_h + *h, true);
		return UPPER_COLLISION;
	}
	else { return NONE_COLLISION; }
}

//returns true if the two rectangles are overlapping
bool j1Collision::CheckCollisionSimplified(SDL_Rect* r1, SDL_Rect* r2)
{
	bool ret = false;
	if (r1 != NULL && r2 != NULL)
	{
		if (r1->x < r2->x + r2->w &&
			r1->x + r1->w > r2->x &&
			r1->y < r2->y + r2->h &&
			r1->h + r1->y > r2->y)
			ret= true;
	}
	return ret;
}

collision_type j1Collision::CalculateFinalCollision(collision_type collision_array[], collision_type current_collision) {
	int collision_count = 0;

	for (int i = 0; i < LAST_COLLISION; i++) {
		switch (collision_array[i]) {
		case NONE_COLLISION:
		case LEFT_COLLISION:
		case RIGHT_COLLISION:
		case UPPER_COLLISION:
		case GROUND_COLLISION:
			collision_count += collision_array[i];
			break;
		}
	}

	return current_collision = collision_array[collision_count];
}

// Gets the buffer collider to avoid the player going into a solid mass
void j1Collision::CollisionPriority(float collider_x, float collider_y, bool horizontal_collider) {

	switch (has_already_collided) {
	case false:
		current_collision_buffer.collider1.x = (int)collider_x;
		current_collision_buffer.collider1.y = (int)collider_y;
		has_already_collided = true;
		if (horizontal_collider == true) { current_collision_buffer.is_first_collider_horizontal = true; }
		break;
	case true:
		current_collision_buffer.collider2.x = (int)collider_x;
		current_collision_buffer.collider2.y = (int)collider_y;
		break;
	}
}

collider_type j1Collision::GetCurrentCollider(int id) {
	if (id == 0) { return regular_collider;	}
	else if (id == 1) { return grenade_collider;	}
	else if (id == 2) { return door_collider;	}
	else if (id == 3) { return under_platform_collider; }
	else if (id == 4) { return starting_point;	}
	else if (id == 5) { return death_collider;	}
	else if (id == 6) { return enemy_elemental; }
	else if (id == 7) { return enemy_horse; }
	else if (id == 8) { return enemy_skull; }
	else { return unknown_collider; }
}

//Called from the Map module every time a map is loaded
void j1Collision::SetPointerToObjGroup(p2List<ObjectGroup*> &pointerObjGroupList)
{
	pObjGroupList = pointerObjGroupList;
}

bool j1Collision::InsideCollider(p2List_item<Object*>* currentobj, float* x, float* y, float* w, float* h) {
	float collider_x = (float)currentobj->data->boundingbox.x;
	float collider_y = (float)currentobj->data->boundingbox.y;
	float collider_w = (float)currentobj->data->boundingbox.w;
	float collider_h = (float)currentobj->data->boundingbox.h;
	SDL_Rect rect = App->entity_m->grenade->anim.GetCurrentFrame()->animationRect;
	if (*x >= collider_x - rect.w && *x + *w <= collider_x + collider_w + rect.w && *y >= collider_y - rect.h && *y + *h <= collider_y + collider_h + rect.h) {
		return true;
	}
	else { return false; }
}

bool j1Collision::DeathColliderTouched() {
	if (death_collider_touched == true) { return true; }
	else { return false; }
}

bool j1Collision::DoorColliderTouched() {
	if (door_collider_touched == true) { return true; }
	else { return false; }
}

bool j1Collision::GrenadeColliderTouched() {
	if (grenade_collider_touched == true) { return true; }
	else { return false; }
}

bool j1Collision::GroundCollision() {
	if (current_collision == GROUND_COLLISION || current_collision == LEFT_GROUND_COLLISION || current_collision == RIGHT_GROUND_COLLISION) {
		return true;
	}
	else { return false; }
}

bool j1Collision::LeftCollision() {
	if (current_collision == LEFT_COLLISION || current_collision == LEFT_GROUND_COLLISION || current_collision == LEFT_UPPER_COLLISION) {
		return true;
	}
	else { return false; }
}

bool j1Collision::RightCollision() {
	if (current_collision == RIGHT_COLLISION || current_collision == RIGHT_UPPER_COLLISION || current_collision == RIGHT_GROUND_COLLISION) {
		return true;
	}
	else { return false; }
}

void j1Collision::PrintColliders() {
	p2List_item<ObjectGroup*>* itemOG = pObjGroupList.start;
	collider_type aux_collider_type;
	SDL_Rect rect;
	uint alpha = 80;

	while (itemOG != NULL)
	{
		p2List_item<Object*>* itemO = itemOG->data->objlist.start;
		while (itemO != NULL)
		{
			aux_collider_type = App->collision->GetCurrentCollider(itemO->data->type);
			rect.x = itemO->data->boundingbox.x;
			rect.y = itemO->data->boundingbox.y;
			rect.w = itemO->data->boundingbox.w;
			rect.h = itemO->data->boundingbox.h;
			switch (aux_collider_type) {
			case death_collider:
				App->render->DrawQuad(rect, 0, 0, 0, alpha);
				break;
			case regular_collider:
				App->render->DrawQuad(rect, 0, 255, 0, alpha);
				break;
			case grenade_collider:
				App->render->DrawQuad(rect, 255, 0, 0, alpha);
				break;
			case door_collider:
				App->render->DrawQuad(rect, 0, 0, 255, alpha);
				break;
			case starting_point:
				App->render->DrawQuad(rect, 50, 50, 50, alpha);
				break;
			}

			itemO = itemO->next;
		}
		itemOG = itemOG->next;
	}
}

bool j1Collision::CheckWalkability(iPoint& tile) {
	p2List_item<ObjectGroup*>* itemOG = pObjGroupList.start;
	collider_type aux_collider_type;
	SDL_Rect coll_rect;

	while (itemOG != NULL) {
		p2List_item<Object*>* itemO = itemOG->data->objlist.start;
		while (itemO != NULL) {
			aux_collider_type = App->collision->GetCurrentCollider(itemO->data->type);
			switch (aux_collider_type) {
			case death_collider:
			case regular_collider:
			case grenade_collider:
			case under_platform_collider:
				coll_rect.x = itemO->data->boundingbox.x / App->map->data.tile_width;
				coll_rect.y = itemO->data->boundingbox.y / App->map->data.tile_height;
				coll_rect.w = itemO->data->boundingbox.w / App->map->data.tile_width - 1;
				coll_rect.h = itemO->data->boundingbox.h / App->map->data.tile_height - 1;
				if (coll_rect.x <= tile.x && coll_rect.x + coll_rect.w >= tile.x
					&& coll_rect.y <= tile.y && coll_rect.y + coll_rect.h >= tile.y) {
					return false;
				}
				break;
			} itemO = itemO->next;
		} itemOG = itemOG->next;
	} return true;
}
