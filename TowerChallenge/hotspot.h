#pragma once

#include "framework.h"
#include "colordefs.h"
#include "MathUtils.h"
#include "log.h"
#include "rect2d.h"
#include <vector>
#include <string>



class _hspots
{
public:

	Rect2D rec;
	int active;
	int mouseover;
	int clicked;

	
	//_hspots() : x(0), y(0), width(0), height(0),color(0) { }
	//_hspots(float x, float y, uint32_t _color) : x(x), y(y), width(width), height(width), color(_color) {}
	//_hspots(const Vec2& c, const Vec2& s,const uint32_t& _color) : x(c.x), y(c.y), width(s.x), height(s.y),color(_color) {}


	_hspots(const  Rect2D &r) : rec(r), active(0),  mouseover(0), clicked(0) {}
};


class HotSpot
{

public:
	//Constructor	
	HotSpot();
	//Destructor
	~HotSpot();
	//Public Variables

	void AddRect( Rect2D &r);
	void Update();
	void rect_from_center(float x, float y, float width, float height);
	void Update_mouse_rect (float x, float y, float width, float height);

private:
	//Private Variables
	std::vector<_hspots> hspots;
	Rect2D m_pos;
	
	



};
