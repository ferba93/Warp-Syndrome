#ifndef __STATIC_TEXT_H__
#define __STATIC_TEXT_H__
#include "SDL/include/SDL.h"
#include "UI_Elements.h"
#include "j1Fonts.h"

class Static_Text :public UI
{
public:
	Static_Text(float x, float y, UI* node, const char* text_input, Uint32 p_width, UI_Purpose second_type = PURPOSE_UNSPECIFIED, int* counter = nullptr, int mask_height = 0, int r = 255, int g = 255, int b = 255, int a = 255);
	virtual ~Static_Text();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void NewText(const char* text);

private:
	_TTF_Font* font;
	Uint32 row_w;
	int* counter_path;
	int max_height;
};

#endif // !__STATIC_TEXT_H__
