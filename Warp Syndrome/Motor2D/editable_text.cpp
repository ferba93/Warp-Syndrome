#include "j1App.h"
#include "editable_text.h"
#include "j1GUI.h"
#include "j1Textures.h"
#include "j1Render.h"

Editable_Text::Editable_Text(float x, float y, UI* node, float width, bool focus) : UI(x, y, parent) {
	texture_section = { 494, 577, (int)width + 10, 45} ;
	rect = { 0, 0, 0, 0};
	cursor = { (int)x, (int)y, 1, 5};
	font = App->font->fonts.start->data;
	max_width = width;
	type = UI_TYPE_EDITABLE_TEXT;
}

Editable_Text::~Editable_Text() {}

bool Editable_Text::PreUpdate() {

	return true;
}

bool Editable_Text::Update(float dt) {

	for (int i = 0; i < CHAR_ARRAY; i++) {
		text[i] = App->input->text[i];
	}
	// Move cursor

	return true;
}

bool Editable_Text::PostUpdate() {

	// Measurements
	text_texture = App->font->Print(text, {}, font);
	App->font->CalcSize(text, rect.w, rect.h);
	texture_section.h = rect.h + 10;
//	cursor.x	Calculate the new x
	cursor.h = rect.h;

	// Blits
	App->render->Blit(texture, position.x - 5, position.y - 5, &texture_section);
	App->render->Blit(text_texture, position.x, position.y, &rect);
	App->render->DrawQuad(cursor, 255, 255, 255);
	
	App->tex->UnLoad(text_texture);

	return true;
}

bool Editable_Text::CleanUp() {

	return true;
}
