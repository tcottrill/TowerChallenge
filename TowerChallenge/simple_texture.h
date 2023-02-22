
#pragma once

#ifndef SIMPLE_TEX_H
#define SIMPLE_TEX_H


#include "glew.h"
#include <string>
#include <vector>
#include <map>

class TEX
{
public:

	GLuint texid;
	int width;
	int height;
	int comp;
	std::string name;
	std::string archive;
	bool loaded;
	
	TEX() : texid(0), width(0), height(0), comp(0), name("NA"), archive("") , loaded(true){ }

	~TEX();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TEX(std::string const &filename, std::string const &archname = std::string(), int numcomponents = 0, int filter = 0);
	
	void UseTexture(GLboolean linear, GLboolean mipmapping);
	void SetBlendMode(int mode);
	int GetTexID();

private:


};

void SnapShot();
#endif