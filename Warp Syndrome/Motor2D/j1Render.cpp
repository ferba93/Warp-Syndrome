#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "Player.h"
#include "j1Scene.h"
#include "j1Grenade.h"

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	App->vSyncActivated = false;

	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
	flags |= SDL_RENDERER_PRESENTVSYNC;
	LOG("Using vsync");
	App->vSyncActivated = true;
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);
	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	App->render->currentcam.x = App->render->camera.x;
	App->render->currentcam.y = App->render->camera.y;
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	return true;
}

bool j1Render::PostUpdate()
{
	App->render->camera.x = App->render->currentcam.x;
	App->render->camera.y = App->render->currentcam.y;
	if (App->scene->blit_colliders == true) {
		App->collision->PrintColliders();
		PrintPlayerObjects();
	}
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	currentcam.x = data.child("camera").attribute("x").as_int(0);
	currentcam.y = data.child("camera").attribute("y").as_int(0);

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool fliped,float pivotX,float pivotY, float speedX, float speedY, double angle, int rotpivot_x, int rotpivot_y) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speedX) + x * scale;
	rect.y = (int)(camera.y * speedY) + y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point rotpivot;

	if (rotpivot_x != INT_MAX && rotpivot_y != INT_MAX)
	{
		rotpivot.x = rotpivot_x;
		rotpivot.y = rotpivot_y;
		p = &rotpivot;
	}

	if (fliped)
	{
		rect.x = rect.x - rect.w + pivotX;
		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_HORIZONTAL) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else
	{
		rect.x = rect.x - pivotX;
		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0) { ret = false; }
	}
	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

void j1Render::PrintPlayerObjects() {
	SDL_Rect rect;
	uint alpha = 80;
// Blits player
	rect.x = App->entity_m->player->pos.x - App->entity_m->player->hitbox_w_h.x / 2;
	rect.y = App->entity_m->player->pos.y - App->entity_m->player->hitbox_w_h.y;
	rect.w = App->entity_m->player->hitbox_w_h.x;
	rect.h = App->entity_m->player->hitbox_w_h.y;
	DrawQuad(rect, 255, 255, 0, alpha);
// Blits grenade
	if (App->entity_m->grenade!=nullptr) {
		rect.x = App->entity_m->grenade->anim.GetCurrentFrame()->animationRect.x;
		rect.y = App->entity_m->grenade->anim.GetCurrentFrame()->animationRect.y;
		rect.w = App->entity_m->grenade->anim.GetCurrentFrame()->animationRect.w;
		rect.h = App->entity_m->grenade->anim.GetCurrentFrame()->animationRect.h;
		DrawQuad(rect, 255, 255, 255, alpha);
	}
}
