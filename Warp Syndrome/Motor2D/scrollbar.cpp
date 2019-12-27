#include "j1App.h"
#include "scrollbar.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Audio.h"

Scrollbar::Scrollbar(float x, float y, UI* node, float length, UI_Purpose secondary_type) : UI(x, y, node) {
	initial_point = y;
	max_point = initial_point + length;
	current_point = y;
	texture_section.x = 971;
	texture_section.y = 786;
	texture_section.w = 13;
	texture_section.h = length + 20;	// max length of 138 + 20 in the atlas (the +20 is the bar height)
	bar_measures = { 1001, 880, 20, 20 };
	initial_mouse_pos = -1;
	purpose_type = secondary_type;
	type = UI_TYPE_SLIDER;
	if (secondary_type == SCROLLBAR_MUSIC) { current_point = (App->audio->music_volume * max_point / 100) + initial_point; }
	else if (secondary_type == SCROLLBAR_SFX) { current_point = (App->audio->fx_volume * max_point / 100) + initial_point; }
}

Scrollbar::~Scrollbar() {

}

bool Scrollbar::Update(float dt) {

	if (Pressed() == true) {
		if (initial_mouse_pos == -1) {
			iPoint posi;
			App->input->GetMousePosition(posi.x, posi.y);
			initial_mouse_pos = posi.y;
		}
		Move();
	} else { initial_mouse_pos = -1; }

	return true;
}

bool Scrollbar::PostUpdate() {

	if (purpose_type == SCROLLBAR_MUSIC) {
		int volume = (current_point - initial_point) * 100 / (max_point - initial_point);
		App->audio->music_volume = volume;
		App->audio->SetVolume(volume, 0);
	}
	else if (purpose_type == SCROLLBAR_SFX) {
		int volume = (current_point - initial_point) * 100 / (max_point - initial_point);
		App->audio->fx_volume = volume;
		App->audio->SetVolume(volume, 1);
	}


	App->render->Blit(texture, position.x, position.y, &texture_section,false,0.0f,0.0f,0.0f,0.0f);
	App->render->Blit(texture, position.x, current_point, &bar_measures, false, 0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

bool Scrollbar::CleanUp() {
	return true;
}

bool Scrollbar::Move() {
	iPoint posi;
	App->input->GetMousePosition(posi.x, posi.y);
	if (posi.y < initial_point) {
		current_point = initial_point;
		return false;
	}
	if (posi.y > max_point) {
		current_point = max_point;
		return false;
	}
	current_point = posi.y;
}