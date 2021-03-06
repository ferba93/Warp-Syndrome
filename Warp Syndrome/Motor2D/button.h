#ifndef __BUTTON_H__
#define __BUTTON_H__
#include "SDL/include/SDL.h"
#include "UI_Elements.h"

class Button :public UI
{
public:
	Button(float x, float y, UI* node, UI_Purpose secondary_type);
	virtual ~Button();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:
	bool pushed;
};

#endif // !__BUTTON_H__
