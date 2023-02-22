///
/// WARNING!!! All Test assume an opengl origin 0,0 on bottom left corner. 
/// 
/// Most code here is from https://github.com/jeffThompson/CollisionDetection
/// License: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
/// 

#pragma once

#include "MathUtils.h"
#include "circle.h"
#include "rect2d.h"
#include <vector>


// CIRCLE/CIRCLE Collision Test
bool circleCircle(Circle& c1, Circle& c2);
// RECTANGLE/RECTANGLE Collision Test
bool rectRect(const Rect2D& r1, const Rect2D& r2);
// CIRCLE/RECTANGLE Collision Test
bool circleRect(Circle& c, Rect2D& r);
// Polygon Collision Start 
bool checkCollisionOneSided(vector<Vec2>& object1, vector<Vec2>& object2);
// Check Polygon Collision
bool isColliding(vector<Vec2>& object1, vector<Vec2>& object2);
// RECT to Poly Collision, converting the rect to a poly and checking
bool rect_to_poly_collision(Rect2D& f, vector<Vec2>& second);
// Calculate an axis aligned bounding box around a polygon for broad phase collison detection, speed up collison testing on polygons
Rect2D Rectangle_Test(vector<Vec2>& poly);
// Line testing
void calculate_line_point(int x1, int y1, int x2, int y2, int distance);
// Find the distance between a point along a line and the endpoint
float dist(float px, float py, float x1, float y1);
// LINE/POINT Collision
bool linePoint(float x1, float y1, float x2, float y2, float px, float py);
// POINT/CIRCLE Collision
bool pointCircle(float px, float py, float cx, float cy, float r);
// LINE/CIRCLE Collision
bool lineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r);