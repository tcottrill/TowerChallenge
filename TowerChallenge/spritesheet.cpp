/*
BASIC Sprite example implementation for C++ and OpenGL 2.0+

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
--------------------------------------------------------------------------------
This program uses textures and parses generic XML/JSON from this program:
 http://www.codeandweb.com/texturepacker
Currently it doesn't support sprite offset and rotation.

Although I'm giving this away, I'd appreciate an email with fixes or better code!

aaedev@gmail.com 2016
*/


//#include <windows.h>
#include "Spritesheet.h"

#include "log.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <iterator>       // std::distance
#include <stdexcept>      // std::out_of_range
#include <algorithm>
#include <vector> 



#include <chrono>

using namespace std;

#pragma warning (disable : 4996 )
typedef std::chrono::high_resolution_clock Clock2;

std::vector<std::string> split(const std::string& text, const std::string& delims)
{
	std::vector<std::string> tokens;
	std::size_t start = text.find_first_not_of(delims), end = 0;

	while ((end = text.find_first_of(delims, start)) != std::string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = text.find_first_not_of(delims, end);
	}
	if (start != std::string::npos)
		tokens.push_back(text.substr(start));

	return tokens;
}

/**
Parses the Sprite Sheet definition file and reads the specified keywords.
This could be broken out in other, more robust token parsing code.
*/
bool Sprite::ParseDefinition(const std::string& fontfile)
{

	int s_index = 0;
	string frm = "frame";
	std::ifstream Stream(fontfile.c_str());
	std::stringstream LineStream;
	//Temporary Sprite 
	SpriteDescriptor C;

	wrlog("Sprite Def here is %s", fontfile.c_str());

	LineStream << Stream.rdbuf();

	string delimiter = "{:},";
	vector<string> v = split(LineStream.str(), delimiter);

	for (auto it = begin(v); it != end(v); ++it)
	{
		//Remove Quotes
		it->erase(remove(it->begin(), it->end(), '\"'), it->end());
		//Remove Whitespace
		it->erase(remove_if(it->begin(), it->end(), isspace), it->end());

		//wrlog("line %s", it->c_str());

	}

	//Now parse the data, warning, no error checking in this code!
	for (int i = 0; i < v.size(); i++)
	{
		std::string::size_type sz;

		if (v[i].compare("frame") == 0)
		{
			s_index = i;
			C.name = v[s_index - 2];
			C.x = std::stoi(v[s_index + 3], &sz);
			C.y = std::stoi(v[s_index + 5], &sz);
			C.Width = std::stoi(v[s_index + 7], &sz);
			C.Height = std::stoi(v[s_index + 9], &sz);
			C.spOffset = spoint(0, 0);
			if (v[s_index + 11].compare("false") == 0)
			{
				C.rot = 0;
			}
			else
			{
				C.rot = 1;
			}

			Sprites.emplace_back(C);
			count++;
		}
		if (v[i].compare("image") == 0)
		{
			Pngname = v[i + 1];
		}

		if (v[i].compare("size") == 0)
		{
			sheetWidth = std::stoi(v[i + 3], &sz);
			sheetHeight = std::stoi(v[i + 5], &sz);
		}
	}


	Stream.close();
	wrlog("Finished sprite load");
	return true;
}


/**
Simple loader that checks if the file exists and then loads it and the associated png
*/
bool  Sprite::LoadSheet(std::string fontfile)
{
	if (!datapath.empty())
	{
		fontfile = datapath + fontfile;
	}

	wrlog("Starting to parse Sprite Def file: %s", fontfile.c_str());

	std::ifstream Stream(fontfile);
	if (!Stream.is_open())
	{
		wrlog("Cannot Find Sprite Definition File %s", fontfile.c_str());
		Stream.close();
		return false;
	}
	Stream.close();

	wrlog("Passing Sprite Load");
	wrlog("Starting to Parse XML %s", fontfile.c_str());
	ParseDefinition(fontfile);
	wrlog("Finished Parsing XML %s", fontfile.c_str());

	if (!datapath.empty())
	{
		// Yes I realize this is ineffecient 
		Pngname = datapath + Pngname;
	}

	//ftexid = LoadPNG(Pngname);
	ftexid = new TEX(Pngname);
	if (!ftexid)
	{
		wrlog("Cannot find font texture for loading %s", fontfile.c_str());
		return false;
	}

	wrlog("File name %s", Pngname.c_str());
	wrlog("Sheetwidth %d", sheetWidth);
	wrlog("Sheetheight %d", sheetHeight);
	wrlog("Sprite count %d", count);
	wrlog("SPRITES:\n");

	for (int i = 0; i < count; ++i)
	{
		wrlog("Sprite number %d", i);
		wrlog("Sprite name: %s", Sprites[i].name.c_str());
		wrlog("Sprite x:%d y %d", Sprites[i].x, Sprites[i].y);
		wrlog("Sprite width:%d height %d", Sprites[i].Width, Sprites[i].Height);
		wrlog("Sprite Rotated %d\n", Sprites[i].rot);
	}

	//Pre allocate enough memory in the Texture list to account for all the sprite data.
	//txlist.reserve(count * 6);

	return true;
}

//Simple Vector Renderer Needs moved to it's own file and class for real world use. 
void Sprite::Render()
{
	//Select and enable the font texture. (With mipmapping.)
	//UseTexture(&ftexid->texid, 1, 1);
	//Set type of blending to use with this font.
	//SetBlendMode(fblend);
	//Select and enable the font texture. (With mipmapping.)
	ftexid->UseTexture(1, 1);
	//Set type of blending to use with this font.
	ftexid->SetBlendMode(fblend);

	//Enable Client States
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(stxdata), &txlist[0].x);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(stxdata), &txlist[0].tx);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(stxdata), &txlist[0].colors);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)txlist.size());

	// Finished Drawing, disable client states.
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//wrlog("Txtlist size here %d", txlist.size());
	txlist.resize(0);
}


void Sprite::MathTableInit()
{
	for (int a = 0; a < 360; a++)
	{
		_sin[a] = (float)sin((float)a * 3.14159265358979323846264f / 180.0f);
		_cos[a] = (float)cos((float)a * 3.14159265358979323846264f / 180.0f);
	}
}


void Sprite::SetCaching(bool en)
{
	if (en) fcache = true;
	else fcache = false;
}


void Sprite::SetScale(int spritenum, float newscale)
{
	if (spritenum < count + 1)
	{
		if (newscale > 0.1f && newscale < 10.0f)
		{
			Sprites[spritenum].scale = newscale;
		}
	}
}


/**
This is the main printing process that scales and fills in the vector structure to be printed later.
*/
void Sprite::InternalAdd(float x, float y, int spriteid)
{
	SpriteDescriptor* f;		   //Pointer to sprite.
	float MinX = 0.0f;
	float MinY = 0.0f;
	float MaxX = 0.0f;
	float MaxY = 0.0f;
	int Flen = 0;
	float s = 0.0f;
	float t = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
	spoint center;

	auto t1 = Clock2::now();


	//Get the current sprite being processed.
	f = &Sprites[spriteid];

	//Get Origin point
	 center = spoint(x, y);

	//Makes sure the x and y placement doesn't move with scaling.
	x = x * (1.0f / f->scale);
	y = y * (1.0f / f->scale);

	//Is there an offset adjustment needed?
	if (f->spOffset.x != 0 || f->spOffset.y != 0)
	{
		center.x += f->spOffset.x;
		center.y += f->spOffset.y;
	}

	//Set Origin to Center of Texture
	MinX = x - (f->Width / 2.0f);
	MinY = y - (f->Height / 2.0f);
	MaxX = x + (f->Width / 2.0f);
	MaxY = y + (f->Height / 2.0f);

	//And set the Scaling
	MinX *= f->scale;
	MinY *= f->scale;
	MaxX *= f->scale;
	MaxY *= f->scale;

	//Add scaling centering here?

	//Calculate texture coords for this sprite
	//This could be pre-calculated.
	s = (float)f->x / sheetWidth;
	t = (float)1.0f - (f->y / (float)sheetHeight);
	w = (float)f->Width / sheetWidth;
	h = (float)f->Height / (float)sheetHeight;

	

	//If we are printing at an angle, there are a lot more calculations.

	if (f->angle)
	{

		//Build our rotated vertexes
		spoint p0 = RotateAroundPoint(MinX, MinY, center.x, center.y, _cos[f->angle], _sin[f->angle]);
		spoint p1 = RotateAroundPoint(MaxX, MinY, center.x, center.y, _cos[f->angle], _sin[f->angle]);
		spoint p2 = RotateAroundPoint(MaxX, MaxY, center.x, center.y, _cos[f->angle], _sin[f->angle]);
		spoint p3 = RotateAroundPoint(MinX, MaxY, center.x, center.y, _cos[f->angle], _sin[f->angle]);

		//Emplace back the vertexes as triangle data 
		//V0
		txlist.emplace_back(p0.x, p0.y, s, t - h, fcolor);
		//V1
		txlist.emplace_back(p1.x, p1.y, s + w, t - h, fcolor);
		//V2
		txlist.emplace_back(p2.x, p2.y, s + w, t, fcolor);
		//V2
		txlist.emplace_back(p2.x, p2.y, s + w, t, fcolor);
		//V3
		txlist.emplace_back(p3.x, p3.y, s, t, fcolor);
		//V0
		txlist.emplace_back(p0.x, p0.y, s, t - h, fcolor);
	}
	else
	{   //Emplace back the non-rotated vertexes as triangle data 
		//V0 0,0
		txlist.emplace_back(MinX, MinY, s, t - h, fcolor);
		//V1 1,0
		txlist.emplace_back(MaxX, MinY, s + w, t - h, fcolor);
		//V2 1,1
		txlist.emplace_back(MaxX, MaxY, s + w, t, fcolor);
		//V2 1,1
		txlist.emplace_back(MaxX, MaxY, s + w, t, fcolor);
		//V3 0,1
		txlist.emplace_back(MinX, MaxY, s, t, fcolor);
		//V0 0,0
		txlist.emplace_back(MinX, MinY, s, t - h, fcolor);
	}

	//auto t2 = Clock2::now();
	//wrlog("Time here in sprite add is %d", t2 - t1);
}


std::string Sprite::numToName(int spritenum)
{
	try {
		auto it = Sprites.at(spritenum);      // vector::at throws an out-of-range
		return it.name;
	}

	catch (const std::out_of_range& err)
	{
		wrlog("Out of Range error: get spritename: %s \n", err.what());
	}
	return ("notfound");
}


int Sprite::nameToNum(std::string name)
{
	for (auto i = Sprites.begin(); i != Sprites.end(); ++i)
	{
		if (name == i->name) { return i - Sprites.begin(); }
	}

	wrlog("Spritename: %s not found, returning 0\n", name.c_str());
	return 0;
}


//Apply a sprite Origin offset for rotation/placement purposes. (Discreet X-Y values)
void Sprite::setOriginOffset(float x, float y, int spritenum)
{
	if (spritenum < count + 1)
	{
		Sprites[spritenum].spOffset.x = x;
		Sprites[spritenum].spOffset.y = y;
	}
}


//Apply a sprite Origin offset for rotation/placement purposes. (vector point)
void Sprite::setOriginOffset(spoint a, int spritenum)
{
	if (spritenum < count + 1)
	{
		Sprites[spritenum].spOffset.x = a.x;
		Sprites[spritenum].spOffset.y = a.y;
	}
}


//Clip Rotation
void Sprite::setRotationAngle(int spritenum, int sangle)
{
	if (spritenum < count + 1)
	{
		Sprites[spritenum].angle = (sangle % 360 + 360) % 360;
	}
}


//Pathing Code, prob should be it's own class
void Sprite::setPath(const char* path)
{
	datapath.assign(path);
}

void Sprite::setPath(std::string& path)
{
	datapath.assign(path);
}


std::string Sprite::getPath()
{
	return datapath;
}


void Sprite::Add(float x, float y, int spritenum)
{
	SetScale(spritenum, 1.0f);
	InternalAdd(x, y, spritenum);
}


void Sprite::Add(float scale, float x, float y, int spritenum)
{

	SetScale(spritenum, scale);
	InternalAdd(x, y, spritenum);
}


void Sprite::Add(float x, float y, rgb_t color, float scale, int spritenum)
{
	SetScale(spritenum, scale);
	SetColor(color);
	InternalAdd(x, y, spritenum);
}


void Sprite::Add(float x, float y, rgb_t color, float scale, int angle, int spritenum)
{
	setRotationAngle(spritenum, angle);
	SetScale(spritenum, scale);
	SetColor(color);
	InternalAdd(x, y, spritenum);
}


// Class Destructor
Sprite::~Sprite()
{
	Sprites.clear();
	txlist.clear();
	//FreeTexture(ftexid);
}


//Rotation Handler
Sprite::spoint Sprite::RotateAroundPoint(float x, float y, float cx, float cy, float cosTheta, float sinTheta)
{
	spoint p;
	p.x = ((((x)-(cx)) * (cosTheta)) - (((y)-(cy)) * (sinTheta))) + (cx);
	p.y = ((((x)-(cx)) * (sinTheta)) + (((y)-(cy)) * (cosTheta))) + (cy);
	return p;
}

