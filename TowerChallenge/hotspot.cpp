
#include "hotspot.h"


using namespace std;




HotSpot::~HotSpot()
{
	wrlog("Class Destructor called");
}


HotSpot::HotSpot()
{
	//Init Stuff Here
	rgb_t color = RGB_WHITE;

};

void HotSpot::Update_mouse_rect(float x, float y, float width, float height)
{
	float minx = 0.0f;
	float miny = 0.0f;
	float maxx = 0.0f;
	float maxy = 0.0f;

	//Set Origin to Center of Texture
	minx = x - (width / 2.0f);
	miny = y - (height / 2.0f);
	maxx = x + (width / 2.0f);
	maxy = y + (height / 2.0f);
	/*
	glBegin(GL_LINE_LOOP);
	glVertex2f(minx, miny);
	glVertex2f(maxx, miny);
	glVertex2f(maxx, maxy);
	glVertex2f(minx, maxy);
	glEnd();
	*/
	
	m_pos.x = minx;
	m_pos.y = miny;
	m_pos.width = width;
	m_pos.height = height;
}

void HotSpot::AddRect( Rect2D& r)
{

	hspots.emplace_back(r);

	/*

		float MinX = 0.0f;
		float MinY = 0.0f;
		float MaxX = 0.0f;
		float MaxY = 0.0f;

		//Set Origin to Center of Texture
		MinX = r.x - (r.width / 2.0f);
		MinY = r.y - (r.height / 2.0f);
		MaxX = r.width + (r.width / 2.0f);
		MaxY = r.height + (r.height / 2.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2f(MinX, MinY);
		glVertex2f(MaxX, MinY);
		glVertex2f(MaxX, MaxY);
		glVertex2f(MinX, MaxY);
		glEnd();
		*/

}


void HotSpot::Update()
{
	

	for (auto& it : hspots) 
	{


		
	}


}

void HotSpot::rect_from_center(float x, float y, float width, float height) 
{

	Rect2D obj;

	float minx = 0.0f;
	float miny = 0.0f;
	float maxx = 0.0f;
	float maxy = 0.0f;

	//Set Origin to Center of Texture
	minx = x - (width / 2.0f);
	miny = y - (height / 2.0f);
	maxx = x + (width / 2.0f);
	maxy = y + (height / 2.0f);
    /*
	glBegin(GL_LINE_LOOP);
	glVertex2f(minx, miny);
	glVertex2f(maxx, miny);
	glVertex2f(maxx, maxy);
	glVertex2f(minx, maxy);
	glEnd();
	*/
	//obj = &Rect2D(minx, miny, maxx, maxy);
	obj.x = minx;
	obj.y = miny;
	obj.width = width;
	obj.height = height;

	hspots.emplace_back(obj);
}