#include "MathUtils.h"

#pragma once

class Rect2D {
public:

	float x;
	float y;
	float width;
	float height;
	float halfWidth;
	float halfHeight;

	Rect2D() 
	{
		x = 0;
		y = 0;
		width = 0;
		height = 0;
		halfWidth = 0;
		halfHeight = 0;
	}

	Rect2D( float _x, float _y, float _width, float _height ) {
		x = _x;
		y = _y;
		width = _width;
		height = _height;
		halfWidth = _width / 2.0f;
		halfHeight = _height / 2.0f;
	}

	Rect2D( Vec2 p, Vec2 s ) {
		x = p.x;
		y = p.y;
		width = s.x;
		height = s.y;
		halfWidth = s.x / 2.0f;
		halfHeight = s.y / 2.0f;

	}

	bool contains( Vec2 p ) {
		return p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height;
	}

	void setX( float _x ) { x = _x; }
	void setY( float _y ) { y = _y; }
	void setWidth( float _width ) { width = _width; }
	void setHeight( float _height ) { height = _height; }
	void setPosition( Vec2 position ) { x = position.x; y = position.y; }
	void setSize( Vec2 lowerLeft, Vec2 upperRight ) {
		x = lowerLeft.x;
		y = lowerLeft.y;
		width = upperRight.x - x;
		height = upperRight.y - y;
	}
    /* Extend the boundaries of the rect so they contain the given point. */
	void extendTo( Vec2 p ) {
		if( p.x < x ) x = p.x;
		if( p.y < y ) y = p.y;
		if( p.x > x + width ) width = p.x - x;
		if( p.y > y + height ) height = p.y - y;
	}

	float getX() { return x; }
	float getY() { return y; }
	float getWidth() { return width; }
	float getHeight() { return height; }
	Vec2 getPosition() { return Vec2( x, y ); }

private:

};
