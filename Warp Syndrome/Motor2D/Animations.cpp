#include "Animations.h"

Animations::Animations()
{

	currentanimframe =nullptr;
	animationfinished = false;
	numberofFrames = 0;
	animationloop = false;
	animationname = "";
	animationframes.clear();
}

Animations::Animations(p2SString aName, bool aLoop, int aNumberOfFrames)
{
	animationname = aName;
	animationloop = aLoop;
	numberofFrames = aNumberOfFrames;
	currentanimframe = nullptr;
	animationfinished = false;
	animationframes.clear();
}


Animations::~Animations()
{
	CleanUp();
}

bool Animations::CleanUp()
{
	p2List_item<FrameInfo*>* item;
	item = animationframes.start;
	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	animationframes.clear();
	currentanimframe = nullptr;
	return true;
}


void Animations::AddFrame(float duration, SDL_Rect texturerect, iPoint textureoffset)
{
	FrameInfo* newframe = new FrameInfo;

	newframe->frameduration = duration;
	newframe->textureoffset = textureoffset;
	newframe->animationRect = texturerect;
	newframe->actualduration = 0;
	animationframes.add(newframe);
	numberofFrames++;
	currentanimframe = animationframes.start;
}

FrameInfo* Animations::StepAnimation(float dt)
{
	FrameInfo* ret = currentanimframe->data;
	if (ret->actualduration>= ret->frameduration)//only executes the code once the duration of the frame is max
	{
		ret->actualduration = 0;//restarts the duration

		if (currentanimframe->next != nullptr)//if the next element exists go to the next element
			currentanimframe = currentanimframe->next;
		else if (animationloop)//if the animation can loop return to the start
			currentanimframe = animationframes.start;
		else animationfinished = true;


	}
	ret->actualduration += dt;
	return ret;
}
FrameInfo* Animations::StepAnimation(Animation_state&state,float dt)
{
	state = AS_UNKNOWN;
	FrameInfo* ret = currentanimframe->data;
	if (ret->actualduration >= ret->frameduration)//only executes the code once the duration of the frame is max
	{
		ret->actualduration = 0.0f;//restarts the duration

		if (currentanimframe->next != nullptr) {//if the next element exists go to the next element
			currentanimframe = currentanimframe->next;
			state = AS_UNFINISHED;
		}
		else if (animationloop) {//if the animation can loop return to the start
			currentanimframe = animationframes.start;
			state = AS_UNFINISHED;
		}
		else
		{
			animationfinished = true;
			state = AS_FINISHED;
		}


	}
	ret->actualduration += dt;
	return ret;
}

bool Animations::GetAnimationFinish()
{
	if (animationfinished)
	{
		animationfinished = false;
		return true;
	}
	return false;
}

FrameInfo* Animations::GetFrame(int id)
{
	return animationframes.At(id)->data;
}
FrameInfo* Animations::GetCurrentFrame()
{
	return currentanimframe->data;
}


bool Animations::LoadAnim(pugi::xml_node& animationnode)
{
	pugi::xml_node framenode;
	animationname = animationnode.attribute("name").as_string();
	animationloop = animationnode.attribute("canloop").as_bool();
	for (framenode = animationnode.child("frame"); framenode; framenode = framenode.next_sibling("frame"))
	{
		float duration = framenode.child("duration").attribute("value").as_float();

		SDL_Rect rect;
		rect.x = framenode.child("rectangle").attribute("x").as_int();
		rect.y = framenode.child("rectangle").attribute("y").as_int();
		rect.w = framenode.child("rectangle").attribute("w").as_int();
		rect.h = framenode.child("rectangle").attribute("h").as_int();
		iPoint textureoffset;
		textureoffset.x = framenode.child("pivot").attribute("x").as_int();
		textureoffset.y = framenode.child("pivot").attribute("y").as_int();

		AddFrame(duration, rect, textureoffset);
	}

	//checks if the number of frames loaded corresponds to the number in the xml, if not return false.
	if (numberofFrames != animationnode.attribute("numberofframes").as_int())
		return false;

	return true;

}

p2List_item<Animations*>* Animations::GetAnimFromName(p2SString name, p2List<Animations*>* animlist)
{
	p2List_item<Animations*>* animitem = animlist->start;
	while (animitem != NULL)
	{
		if (animitem->data->animationname == name)
		{
			break;
		}
		animitem = animitem->next;
	}
	return animitem;
}

void Animations::ResetAnimation()
{
	p2List_item<FrameInfo*>* item = animationframes.start;
	while (item!=NULL)
	{
		item->data->actualduration = 0;//resets the duration of each frame.
		item = item->next;
	}



	currentanimframe = animationframes.start;
	animationfinished = false;
}
