#include "j1App.h"
#include "j1GUI.h"
#include "p2Log.h"

j1GUI::j1GUI() : j1Module() { name.create("ui_m"); }

// Destructor
j1GUI::~j1GUI() {}

bool j1GUI::Start() {

//	UI_list.add(new Button());

	return true;
}

bool j1GUI::PreUpdate() {

	p2List_item<UI*>* item = UI_list.start;
	while (item != NULL) {
		item->data->PreUpdate();
		item = item->next;
	}

	return true;
}

bool j1GUI::Update(float dt) {
	
	p2List_item<UI*>* item = UI_list.start;
	while (item != NULL) {
		item->data->Update(dt);
		item = item->next;
	}

	return true;
}

bool j1GUI::PostUpdate() {

	p2List_item<UI*>* item = UI_list.start;
	while (item != NULL) {
		item->data->PostUpdate();
		item = item->next;
	}

	return true;
}

// Called before quitting
bool j1GUI::CleanUp()
{
	LOG("Unloading UIManager");
	p2List_item<UI*>* item = UI_list.start;
	while (item != NULL)
	{
		if (item->data->type != UIType::UI_TYPE_BUTTON)
		{
			item->data->CleanUp();
			RELEASE(item->data);
			UI_list.del(item);
			item = item->prev;
		}
		else item = item->next;

	}
	
	UI_list.clear();

	return true;
}