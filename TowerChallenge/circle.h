#pragma once
#ifndef CIRCLEH
#define CIRCLEH

#include "MathUtils.h"

using namespace std;


class Circle
{
public:
	Circle()
	{
		radius = 1.0;
		_x = 0;
		_y = 0;
	};

	Circle(float x, float y, float radi) { radius = radi; _x = x; _y = y; }
	Circle(Vec2 v, float radi) { _x = v.x; _y = v.y; radius = radi; };

	void setRadius(float r) { radius = r; };

	float getRadius() { return radius; };

	float getArea() { return k_pi * radius * radius; };

	float getDiameter() { return radius * 2; };

	float getCircumference() { return 2 * k_pi * radius; };

	bool isInRectangle(float x, float y)
	{
		return x >= _x - radius && x <= _x + radius && y >= _y - radius && y <= _y + radius;
	}

	//test if coordinate (x, y) is within a radius from coordinate (center_x, center_y)
	bool isPointInCircle(float x, float y)
	{
		if (isInRectangle(x, y))
		{
			float dx = _x - x;
			float dy = _y - y;
			dx *= dx;
			dy *= dy;
			float distanceSquared = dx + dy;
			float radiusSquared = radius * radius;
			return distanceSquared <= radiusSquared;
		}
		return false;
	}

	bool isLineinCircle(Vec2 P1, Vec2 P2)
	{

		length = sqrtf(std::powf(P1.x - P2.y, 2.0) + std::powf(P1.y - P2.y, 2.0));
		if (length > radius)
			return true;
		return false;

	}

	bool isLineinCircle(float sx, float sy, float ex, float ey)
	{

		length = sqrtf(std::powf(sx - ey, 2.0) + std::powf(sy - ey, 2.0));
		if (length > radius)
			return true;
		return false;

	}


	//I want to access these directly
	float radius;
	float _x;
	float _y;


private:
	
	float length = 0;
	
};

#endif