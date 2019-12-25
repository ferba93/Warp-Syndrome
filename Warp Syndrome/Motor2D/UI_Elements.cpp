#include "j1App.h"
#include "UI_Elements.h"
#include "j1GUI.h"
#include "j1Input.h"

//UI=====================================================
UI::UI(float x, float y, UI* node = nullptr) {
	position.x = x;
	position.y = y;
	texture_section = { 0, 0, 0, 0 };
	texture = App->gui->GetAtlas();
	parent = node;
	if (parent != nullptr) { App->gui->focus = nullptr; }
}

UI::~UI() { this->CleanUp(); }

bool UI::Awake() { return true; }

bool UI::Start() { return true; }

bool UI::PreUpdate() { return true; }

bool UI::Update(float dt) { return true; }

bool UI::PostUpdate() { return true; }

bool UI::CleanUp() {
	texture = nullptr;
	return true;
}

bool UI::Pressed() {
	iPoint posi;
	App->input->GetMousePosition(posi.x, posi.y);
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == 2) {
		if (posi.x >= position.x && posi.x <= position.x + texture_section.w && posi.y >= position.y && posi.y <= position.y + texture_section.h) {
			return true;
		}
	}
	return false;
}
