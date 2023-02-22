/*
This is freeand unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non - commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain.We make this dedication for the benefit
of the public at largeand to the detriment of our heirsand
successors.We intend this dedication to be an overt act of
relinquishment in perpetuity of all presentand future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to < http://unlicense.org/>
*/

#define NOMINMAX
#include "game.h"

// Includes
#define NOMINMAX
#include "gl_basics.h"
#include "framework.h"
#include "log.h"
#include "winfont.h"
#include "rawinput.h"
#include "ini.h"
#include <vector>
#include "colordefs.h"
#include "box2d/box2d.h"
#include "simple_texture.h"
#include "spritesheet.h"
#include "os_basic.h"
#include "MathUtils.h"
#include "rotation_table.h"
#include "simpletimer.h"
#include "bmfont.h"
#include "colordefs.h"
#include <chrono>
#include "newfbo.h"
#include "dsoundstream.h"
#include "mixer.h"
#include "rect2d.h"
#include "collision_tests.h"
#include <cstdlib> 

using namespace std;

#define DEBUG_LOG 1

#pragma comment(lib, "Box2D.lib")


multifbo* fbo;
b2World* World;

void CreateBox(b2World* World, float MouseX, float MouseY, float size, int bnum);
void CreateGround(b2World* World, float X, float Y, float CX, float CY);

volatile int cloud_counter = 0;
volatile int ani_counter = 0;
volatile int clock_counter = 60;

/** We need this to easily convert between pixel and real-world coordinates for Box2D*/
static const float SCALE = 32.0f;

//std::unique_ptr<TEX*> boxtex;

Sprite* sp;
BMFont* Snap;
BMFont* OpenSansBlue;

// Vars
static int current_game_screen = 0;
static int show_box = 1;
static int boxnum = 0;
static int last_counter = 0;
static int last_anticounter_dir;
static int anticounter_dir = 0;
static float scale_factor = 1.0;
static int last_fps_counter = 0;
static float last_scale_factor = 1.0f;
static int last_boxnum = 0;
static int block_height = 0;
static int last_block_height = 0;
static int contact_happened = 0;
static int music_on = 0;
static int effects_on = 0;
static float cld_1_cnt = 110.0;
static float cld_2_cnt = 225.0;
static float cld_3_cnt = 400.0;

// For mouse button info - This needs to go in a hotspot /mouse class
static int last_mouse_l = 0;
static int last_mouse_r = 0;

static UINT clock_timer = 0;

vector<int> boxlist;
std::vector<int> items;

b2Body* dynamicBody;

// Collision rects. Please make this into a working Hotspot class
Rect2D game_hspot[8];
Rect2D m_rect;


void CreateBox(b2World* World, float MouseX, float MouseY, float size, int bnum)
{
	b2BodyDef BodyDef;
	BodyDef.position = b2Vec2(MouseX / SCALE, MouseY / SCALE);
	BodyDef.awake = true;
	BodyDef.bullet = true;
	BodyDef.type = b2_dynamicBody;
	b2Body* Body = World->CreateBody(&BodyDef);

	b2PolygonShape Shape;
	Shape.SetAsBox((size / 2) / SCALE, (size / 2) / SCALE);
	b2FixtureDef FixtureDef;
	FixtureDef.density = 1.0f;
	FixtureDef.friction = 0.8f;
	FixtureDef.restitution = .05f; // Bounce
	FixtureDef.shape = &Shape;
	Body->CreateFixture(&FixtureDef);
	Body->SetUserData((void*)bnum);

}

void CreateGround(b2World* World, float X, float Y, float CX, float CY)
{
	b2BodyDef BodyDef;
	BodyDef.position = b2Vec2(X / SCALE, Y / SCALE);
	BodyDef.type = b2_staticBody;

	b2Body* Body = World->CreateBody(&BodyDef);

	b2PolygonShape Shape;
	Shape.SetAsBox((CX / 2) / SCALE, (CY / 2) / SCALE);
	b2FixtureDef FixtureDef;
	FixtureDef.density = 1.0f;
	FixtureDef.friction = .8f;
	FixtureDef.restitution = .05f; // Bounce
	FixtureDef.shape = &Shape;
	Body->CreateFixture(&FixtureDef);
}


// Attempt to detect if a body has stopped moving here
/*
bool isStopped(float delta) {
	bool isMoving = (
		abs(body.getLinearVelocity().x) >= 0.25f ||abs(body.getLinearVelocity().y) >= 0.25f);
	if (isMoving) {
		timeStopped = 0.0f;
		return false;
	}
	else {
		timeStopped += delta;
		return timeStopped >= 0.3f;
	}
}
*/
class MyContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact)
	{
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if (DEBUG_LOG) wrlog("Hey we have a contact %d", bodyUserData);
	}

	void EndContact(b2Contact* contact) {

		if (effects_on) { sample_start(1, 1, 0); }
		// Below is for Reference
		/*
		//check if fixture A was a ball
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if (bodyUserData)
			static_cast<Ball*>(bodyUserData)->endContact();

		//check if fixture B was a ball
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserData)
			static_cast<Ball*>(bodyUserData)->endContact();
			*/
	}
};

// At global scope
MyContactListener myContactListenerInstance;


void set_music(int onoff)
{
	static int music_set = 0;

	if (onoff)
	{
		music_set ^= 1;

		if (music_set)
		{
			sample_start(0, 0, 1);
			sample_set_volume(0, 70);
		}
		else
		{
			sample_stop(0);
		}
	}
}


void rect_from_center(float x, float y, float width, float height, Rect2D* obj)
{
	// Set Origin to Center of Texture
	float minx = x - (width / 2.0f);
	float miny = y - (height / 2.0f);
	float maxx = x + (width / 2.0f);
	float maxy = y + (height / 2.0f);

	obj->x = minx;
	obj->y = miny;
	obj->width = width;
	obj->height = height;

}


void update_mouse_info()
{
	int mx; int my;

	get_mouse_win(&mx, &my);
	my = SCREEN_H - my;

	// Update the collision rect;
	rect_from_center((float)mx, (float)my, 4.0f, 4.0f, &m_rect);

	/*

	//If the left button is down
	if (mouse_b & 1)
	{
		//If it's the first time
		if ((last_mouse_l & 1) != (mouse_b & 1))
		{
			last_mouse_l = mouse_b & 1;
		}
	}
	*/

}


class _box_data
{
public:
	float _x;
	float _y;
	int _boxnum;

	_box_data(float _x, float _y, int _boxnum) : _x(_x), _y(_y), _boxnum(_boxnum) {}
	_box_data() : _x(0), _y(0), _boxnum(0) {}
};

vector<_box_data> box_back;


class track {
public:
	int item;
	int type;


	track(int item, int type) : item(item), type(type) {}

};


void CALLBACK cloud_timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{
	cld_1_cnt++;
	cld_2_cnt += 1.5f;
	cld_3_cnt += .8f;

	if (cld_1_cnt > 700) cld_1_cnt = 0;
	if (cld_2_cnt > 700) cld_2_cnt = 0;
	if (cld_3_cnt > 700) cld_3_cnt = 0;
}


void CALLBACK clock_timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{
	clock_counter--;
	if (clock_counter == 0)
	{
		//If it's the first time, end game;
		clock_counter = 60;
	}
}

void rectangle_from_center(float x, float y, float cx, float cy)
{
	//0,1
	//0,0
	//1,0
	//1,1
	glBegin(GL_QUADS);
	glVertex2f(x - cx / 2, y - cy / 2);
	glVertex2f(x - cx / 2, y + cy / 2);
	glVertex2f(x + cx / 2, y + cy / 2);
	glVertex2f(x + cx / 2, y - cy / 2);
	glEnd();
}



void draw_screen_menu()
{

	//Add hotspot
	rect_from_center(190, 575, 190, 89, &game_hspot[0]);
	rect_from_center(460, 575, 190, 89, &game_hspot[1]);

	int col_l = rectRect(m_rect, game_hspot[0]);
	int col_r = rectRect(m_rect, game_hspot[1]);

	if (col_l)
	{
		// Dont care if we get multiple mouse downs here
		if ((mouse_b & 1) && (last_mouse_l == 0))
		{
			last_mouse_l = 1;
			ani_counter = 550;
			current_game_screen = 1;
			mouse_b = 0;
			show_box = 1;
			boxnum = 0;
			clock_timer = install_timer_ex(clock_timer_callback, MSEC_TO_TIMER(1000));
		}
	}

	if (col_r)
	{
		// Dont care if we get multiple mouse downs here
		if ((mouse_b & 1) && (last_mouse_l == 0))
		{
			last_mouse_l = 1;
			ani_counter = 550;
			current_game_screen = 1;
			show_box = 1;
			mouse_b = 0;
			boxnum = 0;
			clock_timer = install_timer_ex(clock_timer_callback, MSEC_TO_TIMER(1000));
		}
	}
	if (!(mouse_b & 1)) { last_mouse_l = 0; }

	sp->Add((float)SCREEN_W / 2, (float)SCREEN_H / 2, sp->nameToNum("bg_menu"));
	if (col_l) {
		sp->Add(190, 575, RGB_WHITE, 1.0f, 0, 7);
	}
	else {
		sp->Add(190, 575, RGB_WHITE, 1.0f, 0, 6);
	}

	if (col_r) {
		sp->Add(460, 575, RGB_WHITE, 1.0f, 0, 9);
	}
	else {
		sp->Add(460, 575, RGB_WHITE, 1.0f, 0, 8);
	}
}

void draw_screen_game()
{
	rect_from_center(40, 900, 60, 63, &game_hspot[2]);
	rect_from_center(110, 900, 60, 63, &game_hspot[3]);
	rect_from_center(530, 900, 60, 63, &game_hspot[4]);
	rect_from_center(600, 900, 60, 63, &game_hspot[5]);

	int col_music = rectRect(m_rect, game_hspot[3]);
	int col_sound = rectRect(m_rect, game_hspot[2]);
	int col_home = rectRect(m_rect, game_hspot[4]);
	int col_replay = rectRect(m_rect, game_hspot[5]);

	float bx = 0;
	float by = 0;
	int udInt = 0;

	sp->Add(SCREEN_W / 2, SCREEN_H / 2, 0); //Background

	if ((mouse_b & 2) && (last_mouse_r == 0))
	{
		last_mouse_r = 1;
		scale_factor -= .05f;

		// So to scale anything, all bodies have to be destroyed and recreated smaller. 
		for (b2Body* BodyIterator = World->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
		{
			if (BodyIterator->GetType() == b2_dynamicBody)
			{
				b2Fixture* f = BodyIterator->GetFixtureList();
				while (f)
				{
					f = f->GetNext();
					_box_data b;

					b._boxnum = (int)BodyIterator->GetUserData();
					b._x = (float)BodyIterator->GetPosition().x * SCALE;
					b._y = (float)BodyIterator->GetPosition().y * SCALE;

					if (DEBUG_LOG) wrlog("Box getting destroyed %f %f %d", b._x, b._y, b._boxnum);

					box_back.push_back(b);
					World->DestroyBody(BodyIterator);
					Sleep(10);
				}
			}

			if (BodyIterator->GetType() == b2_staticBody)
			{
				World->DestroyBody(BodyIterator);
			}
		}
		for (auto it = box_back.begin(); it != box_back.end(); it++)
		{
			if (DEBUG_LOG) wrlog("Creating boxes here");
			CreateBox(World, it->_x, it->_y, 64.0f * scale_factor, it->_boxnum);

		}
		box_back.clear();

		CreateGround(World, 320, 80, 270 * scale_factor, 32 * scale_factor);
	}
	if (!(mouse_b & 2)) { last_mouse_r = 0; }


	if ((mouse_b & 1) && (last_mouse_l == 0))
	{
		last_mouse_l = 1;

		if (col_music)
		{
			if (music_on)
			{
				sample_stop(0);
				music_on = 0;
			}
			else
			{
				sample_start(0, 0, 1);
				sample_set_volume(0, 70);
				music_on = 1;
			}
		}

		if (col_sound)
		{
			effects_on ^= 1;
		}

		if (show_box)
		{
			boxnum = boxnum + 1;
			if (DEBUG_LOG) wrlog("Boxnum here %d", boxnum);
			CreateBox(World, ani_counter, 815, 64.0 * scale_factor, boxnum);
			show_box = 0;
			last_counter = ani_counter;
			last_anticounter_dir = anticounter_dir;
		}

		if (col_home)
		{
			current_game_screen = 0;
			stop_timer(clock_timer);
			sample_stop(0);
			music_on = 0;
			clock_counter = 60;
			for (b2Body* BodyIterator = World->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
			{
				if (BodyIterator->GetType() == b2_dynamicBody)
				{
					World->DestroyBody(BodyIterator);
				}
			}
		}


		if (col_replay)
		{
			stop_timer(clock_timer);
			Sleep(30);
			last_mouse_l = 1;
			ani_counter = 550;
			current_game_screen = 1;
			show_box = 1;
			mouse_b = 0;
			boxnum = 0;
			clock_timer = install_timer_ex(clock_timer_callback, MSEC_TO_TIMER(1000));

			clock_counter = 60;
			for (b2Body* BodyIterator = World->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
			{
				if (BodyIterator->GetType() == b2_dynamicBody)
				{
					World->DestroyBody(BodyIterator);
				}
			}
		}

	}
	if (!(mouse_b & 1)) { last_mouse_l = 0; }

	// Button handling testing for music
	if (col_music)
	{
		if (music_on)
		{
			sp->Add(110, 900, RGB_WHITE, 1.0f, 0, 21);
		}
		else {
			sp->Add(110, 900, RGB_WHITE, 1.0f, 0, 20);
		}
	}
	else
	{
		if (music_on)
		{
			sp->Add(110, 900, RGB_WHITE, 1.0f, 0, 21);
		}
		else {
			sp->Add(110, 900, RGB_WHITE, 1.0f, 0, 20);
		}
	}

	if (col_sound)
	{

		if (effects_on)
		{
			sp->Add(40, 900, RGB_WHITE, 1.0f, 0, 25);
		}
		else
		{
			sp->Add(40, 900, RGB_WHITE, 1.0f, 0, 24);
		}

	}
	else
	{
		if (effects_on)
		{
			sp->Add(40, 900, RGB_WHITE, 1.0f, 0, 25);
		}
		else
		{
			sp->Add(40, 900, RGB_WHITE, 1.0f, 0, 24);
		}

	}

	if (col_home) { sp->Add(530, 900, RGB_WHITE, 1.0f, 0, 19); } // Home 18/19
	else { sp->Add(530, 900, RGB_WHITE, 1.0f, 0, 18); }



	if (col_replay) {
		sp->Add(600, 900, RGB_WHITE, 1.0f, 0, 23);  // Replay 22/23
	}
	else {
		sp->Add(600, 900, RGB_WHITE, 1.0f, 0, 22);  // Replay 22/23
	}

	// Ground!!!
	sp->Add(SCREEN_W / 2, 30 * (1.0f / scale_factor), RGB_WHITE, scale_factor, 0, 17);

	if (show_box)
	{
		sp->Add(ani_counter, 815, RGB_WHITE, scale_factor, 0, boxlist[(boxnum + 1) & 0x03]);
	}

	// Clouds
	sp->Add(cld_1_cnt, 0, RGB_WHITE, scale_factor, 0, 14);
	sp->Add(cld_2_cnt, 0, RGB_WHITE, scale_factor, 0, 15);
	sp->Add(cld_3_cnt, 0, RGB_WHITE, scale_factor, 0, 16);

	Snap->setScale(.8f);
	Snap->fPrint(300.0, 875.0, RGB_WHITE, .68f, "%d", clock_counter);
}



struct MyUserData
{
	string* name;
	// other data
};


void update_world()
{
	static int last_highest_y = 0;
	static int highest_y = 0;

	World->Step(1 / 20.f, 8, 3);


	if (anticounter_dir) { ani_counter += 15; }
	else { ani_counter -= 15; }

	if (ani_counter < 64) anticounter_dir = 1;
	if (ani_counter > 550) anticounter_dir = 0;

	draw_screen_game();


	int BodyCount = 0;
	for (b2Body* BodyIterator = World->GetBodyList(); BodyIterator != 0; BodyIterator = BodyIterator->GetNext())
	{
		for (b2ContactEdge* edge = BodyIterator->GetContactList(); edge; edge = edge->next)
		{
			// This needs to only be triggered once. Then it needs to be ignored until the next click.

			if (edge->contact->IsTouching() && ((int)BodyIterator->GetUserData() == boxnum))
			{
				if (boxnum != last_boxnum)
				{
					if (show_box == 0)
					{

						show_box = 1;
						ani_counter = last_counter;
						anticounter_dir = last_anticounter_dir;
						last_boxnum = boxnum;
						contact_happened = 1;

						if (effects_on) { sample_start(1, 1, 0); }
					}
				}
			}
		}

		if (BodyIterator->GetType() == b2_dynamicBody)
		{
			// Get the fixturelist for a body

			b2Fixture* f = BodyIterator->GetFixtureList();
			while (f)
			{
				f = f->GetNext();

				if ((BodyIterator->GetPosition().y * SCALE) < 0)
				{
					// wrlog("Body Userdata here is %d", BodyIterator->GetUserData());
					World->DestroyBody(BodyIterator);

					// If is the current box, show the moving one
					int udInt = (int)BodyIterator->GetUserData();

					// Ff (udInt != boxnum)
				//	{
					block_height = static_cast<int>(BodyIterator->GetPosition().y * SCALE);
					if (last_block_height < block_height)
					{
						last_block_height = block_height;

					}
					//}
					if (show_box == 0 && udInt == boxnum)
					{
						show_box = 1;
						ani_counter = last_counter;
						anticounter_dir = last_anticounter_dir;
					}
				}
				else
				{
					if (DEBUG_LOG) wrlog("Block height here %i  %i", last_block_height, static_cast<int>(BodyIterator->GetPosition().y * SCALE));
					sp->Add(SCALE * BodyIterator->GetPosition().x, SCALE * BodyIterator->GetPosition().y, RGB_WHITE, scale_factor, BodyIterator->GetAngle() * (180 / 3.14159265), boxlist[(int)BodyIterator->GetUserData() & 0x03]); //"box_1"));
				}
			}
			++BodyCount;
		}
	}
}

int init_game()
{
	// Initialize vector with strings using push_back 
	boxlist.push_back(2);
	boxlist.push_back(3);
	boxlist.push_back(4);
	boxlist.push_back(5);

	init_mixer(44100, 60);
	load_sample(0, "wav\\ost.ogg");
	load_sample(0, "wav\\wooden_hit_1.ogg");
	load_sample(0, "wav\\wooden_hit_2.ogg");
	load_sample(0, "wav\\wooden_hit_3.ogg");
	/////////////////// END INITIALIZATION ////////////////////////////////////

	fbo = new multifbo(640, 940, 1, 0, fboFilter::FB_LINEAR);

	// Load and Initialize the Fonts
	Snap = new BMFont(SCREEN_W, SCREEN_H);
	Snap->setPath("data\\");
	if (!Snap->loadFont("OpenSansBlue.fnt"))
	{
		MessageBox(NULL, "Error, font file not found, exiting", "File Not Found", MB_ICONERROR | MB_OK);
		//Quit = TRUE;
		//PostQuitMessage(-1);
	}
	LOG_DEBUG("Font Loaded Sucessfully");

	//SetThreadPriority(GetCurrentThread(), ABOVE_NORMAL_PRIORITY_CLASS);
	//SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	LOG_DEBUG("Starting Sprite Load");
	sp = new Sprite();
	sp->setPath("data\\");
	sp->LoadSheet("atlas.json");

	LOG_DEBUG("Ending Sprite Load");
	// Prepare the world //
	// b2Vec2 Gravity(0, -9.8); //normal earth gravity, 9.8 m/s/s straight down!

	World = new b2World(b2Vec2(0.0f, -9));

	CreateGround(World, 320, 80, 270, 32);
	World->SetContactListener(&myContactListenerInstance);

	int j = GetSystemMetrics(SM_CYCAPTION);
	if (DEBUG_LOG) wrlog("Titlebar is %d", j);

	install_timer_ex(cloud_timer_callback, MSEC_TO_TIMER(60));
	
	return 0;
}


void run_game()
{

	update_mouse_info();

	fbo->Use();

	if (current_game_screen == 0)
	{
		draw_screen_menu();
	}
	else
	{
		update_world();
	}

	//Add timer code here

	sp->Render();
	Snap->Render();

	fbo->UnUse();
	fbo->Bind(0, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0, 1); glVertex2f(0.0f, (float)SCREEN_H);
	glTexCoord2f(1, 1); glVertex2f((float)SCREEN_W, (float)SCREEN_H);
	glTexCoord2f(1, 0); glVertex2f((float)SCREEN_W, 0);
	glEnd();

	update_mixer();
}



void draw_rect(int x, int y, int width, int height)
{

	glBegin(GL_QUADS);
	glVertex2i(x, y + height);
	glVertex2i(x, y);
	glVertex2i(x + width, y);
	glVertex2i(x + width, y + height);
	glEnd();
}


void texture_from_center(int texnum, float x, float y, int size)
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texnum);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1); glVertex2f(x - size, y - size);
	glTexCoord2f(1, 0); glVertex2f(x - size, y + size);
	glTexCoord2f(0, 0); glVertex2f(x + size, y + size);
	glTexCoord2f(0, 1); glVertex2f(x + size, y - size);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}


void texture_from_center(int texnum, float x, float y, float w, float h)
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texnum);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	//glTexCoord2f(0, 0); glVertex2f(x - w, y - h);
	//glTexCoord2f(0, 1); glVertex2f(x - w, y + h);
	//glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
	//glTexCoord2f(1, 0); glVertex2f(x + w, y - h);
	glTexCoord2f(1, 1); glVertex2f(x - w, y - h);
	glTexCoord2f(1, 0); glVertex2f(x - w, y + h);
	glTexCoord2f(0, 0); glVertex2f(x + w, y + h);
	glTexCoord2f(0, 1); glVertex2f(x + w, y - h);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}


void rectangle_from_center(float x, float y, float cx, float cy, float rotation, float scale)
{
	//0,1
	//0,0
	//1,0
	//1,1
	glBegin(GL_QUADS);
	glVertex2f(x - cx / 2, y - cy / 2);
	glVertex2f(x - cx / 2, y + cy / 2);
	glVertex2f(x + cx / 2, y + cy / 2);
	glVertex2f(x + cx / 2, y - cy / 2);
	glEnd();
}


void box_from_center(float x, float y, float size, float rotation, float scale)
{

	glBegin(GL_LINE_LOOP);
	glVertex2f(x - size, y - size);
	glVertex2f(x - size, y + size);
	glVertex2f(x + size, y + size);
	glVertex2f(x + size, y - size);
	glEnd();
}


void rot_rect(int texnum, float x, float y, float size, int angle)
{
	float MinX = 0.0f;
	float MinY = 0.0f;
	float MaxX = 0.0f;
	float MaxY = 0.0f;

	//Vertices1.clear();
	//Set Origin to Center of Texture
	MinX = x - (size / 2.0f);
	MinY = y - (size / 2.0f);
	MaxX = x + (size / 2.0f);
	MaxY = y + (size / 2.0f);


	Vec2 p0 = RotateAroundPoint(MinX, MinY, x, y, (float)_cos[angle], (float)_sin[angle]);
	Vec2 p1 = RotateAroundPoint(MaxX, MinY, x, y, (float)_cos[angle], (float)_sin[angle]);
	Vec2 p2 = RotateAroundPoint(MaxX, MaxY, x, y, (float)_cos[angle], (float)_sin[angle]);
	Vec2 p3 = RotateAroundPoint(MinX, MaxY, x, y, (float)_cos[angle], (float)_sin[angle]);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texnum);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	//glTexCoord2f(0, 0); glVertex2f(x - w, y - h);
	//glTexCoord2f(0, 1); glVertex2f(x - w, y + h);
	//glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
	//glTexCoord2f(1, 0); glVertex2f(x + w, y - h);
	glTexCoord2f(1, 1); glVertex2f(p0.x, p0.y);
	glTexCoord2f(1, 0); glVertex2f(p1.x, p1.y);
	glTexCoord2f(0, 0); glVertex2f(p2.x, p2.y);
	glTexCoord2f(0, 1); glVertex2f(p3.x, p3.y);

	glEnd();
	glDisable(GL_TEXTURE_2D);

}

//Left here for reference.
/*
b2Vec2 ConvertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	float32 ratio = float32(tw) / float32(th);
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = viewCenter - extents;
	b2Vec2 upper = viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}
*/




void end_game()
{
	end_mixer();
}