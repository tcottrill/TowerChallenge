///
/// WARNING!!! All Test assume an opengl origin 0,0 on bottom left corner. 
/// 
/// Most code here is from https://github.com/jeffThompson/CollisionDetection
/// License: Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
/// 

#include "collision_tests.h"
#include "log.h"


// CIRCLE/CIRCLE Collision Test
bool circleCircle(Circle& c1, Circle& c2)
{
	// get distance between the circle's centers
	// use the Pythagorean Theorem to compute the distance
	float distX = c1._x - c2._x;
	float distY = c1._y - c2._y;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the sum of the circle's
	// radii, the circles are touching!
	if (distance <= c1.radius + c2.radius) {
		return true;
	}
	return false;
}

// RECTANGLE/RECTANGLE Collision Test
bool rectRect(const Rect2D& r1, const Rect2D& r2)
{
	// are the sides of one rectangle touching the other?

	if (r1.x + r1.width >= r2.x &&    // r1 right edge past r2 left
		r1.x <= r2.x + r2.width &&    // r1 left edge past r2 right
		r1.y + r1.height >= r2.y &&    // r1 top edge past r2 bottom
		r1.y <= r2.y + r2.height) {    // r1 bottom edge past r2 top

		return true;
	}
	return false;
}

// CIRCLE/RECTANGLE Collision Test
bool circleRect(Circle& c, Rect2D& r)
{
	// temporary variables to set edges for testing
	float testX = c._x;
	float testY = c._y;

	// which edge is closest?
	if (c._x < r.x)         testX = r.x;      // test left edge
	else if (c._x > r.x + r.width) testX = r.x + r.width;   // right edge
	if (c._y < r.y)         testY = r.y;      // top edge
	else if (c._y > r.y + r.height) testY = r.y + r.height;   // bottom edge

	// get distance from closest edges
	float distX = c._x - testX;
	float distY = c._y - testY;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the radius, collision!
	if (distance <= c.radius) {
		return true;
	}
	return false;
}

/////////////////// Polygon Collision Start /////////////////////////////////////

bool checkCollisionOneSided(vector<Vec2>& object1, vector<Vec2>& object2)
{
	int nume = object1.size();
	for (int i = 0; i < nume; i++)
	{
		Vec2 edge = object1[(i + 1) % nume] - object1[i];
		Vec2 normal = edge.perp();
		double min1 = numeric_limits<double>::infinity();
		double min2 = min1;
		double max1 = -numeric_limits<double>::infinity();
		double max2 = max1;

		for (int j = 0; j < object1.size(); j++)
		{
			double dot = normal.dot(object1[j]);
			min1 = std::min(min1, dot);
			max1 = std::max(max1, dot);
		}
		for (int j = 0; j < object2.size(); j++)
		{
			double dot = normal.dot(object2[j]);
			min2 = std::min(min2, dot);
			max2 = std::max(max2, dot);
		}

		if (min2 > max1 || min1 > max2)
			return false;
	}
	//wrlog("Collision side True First obj side %d ",nume);
	return true;
}

//Check Polygon Collision
bool isColliding(vector<Vec2>& object1, vector<Vec2>& object2)
{
	//wrlog("calling collision check one sided");
	return checkCollisionOneSided(object1, object2) && checkCollisionOneSided(object2, object1);
}

// RECT to Poly Collision, converting the rect to a poly and checking
bool rect_to_poly_collision(Rect2D& f, vector<Vec2>& second)
{
	vector<Vec2> tobj;

	//Set Origin to Center of Texture
	float minx = f.x;
	float miny = f.y;
	float maxx = f.x + f.width;
	float maxy = f.y + f.height;

	tobj.emplace_back(Vec2(minx, miny));
	tobj.emplace_back(Vec2(maxx, miny));
	tobj.emplace_back(Vec2(maxx, maxy));
	tobj.emplace_back(Vec2(minx, maxy));
	return isColliding(tobj, second);
}

//Calculate an axis aligned bounding box around a polygon for broad phase collison detection, speed up collison testing on polygons
Rect2D Rectangle_Test(vector<Vec2>& poly)
{
	Vec2  min = poly[0];
	Vec2  max = poly[0];

	for (int i = 1; i < poly.size(); i++)
	{
		min.x = Min(min.x, poly[i].x);
		min.y = Min(min.y, poly[i].y);
		max.x = Max(max.x, poly[i].x);
		max.y = Max(max.y, poly[i].y);

	}
	return Rect2D(min, Vec2(max.x - min.x, max.y - min.y));
}

///////////// Line/Point Collision //////////////////////////////////

//The quickest way to calculate a point that lies on a line a given distance away from the end point of the line?Why is this longer /different then the other one?
void calculate_line_point(int x1, int y1, int x2, int y2, int distance)
{
	//calculate a point on the line x1-y1 to x2-y2 that is distance from x2-y2
	float vx = x2 - x1; // x vector
	float vy = y2 - y1; // y vector

	float mag = sqrt(vx * vx + vy * vy); // length

	vx /= mag;
	vy /= mag;

	// calculate the new vector, which is x2y2 + vxvy * (mag + distance).?? (Listed wrong)

	int px = (int)((float)x2 + vx * (float)distance);
	int py = (int)((float)y2 + vy * (float)distance);
}

//Find the distance between a point along a line and the endpoint:
float dist(float px, float py, float x1, float y1)
{
	//Start point - (px, py)
	//End point - (x1, y1)

	//float d = sqrt(   (x1 - px) ^ 2 + (y1 - py) ^ 2);
	//d = sqrt((x1 - x0)^2 + (y1 - y0)^2)

	float  distancex = (x1 - px) * (x1 - px);
	float  distancey = (y1 - py) * (y1 - py);
	float d = sqrt(distancex + distancey);

	return d;
}

// LINE/POINT Collision
bool linePoint(float x1, float y1, float x2, float y2, float px, float py)
{
	// get distance from the point to the two ends of the line
	float d1 = dist(px, py, x1, y1);
	float d2 = dist(px, py, x2, y2);

	// get the length of the line
	float lineLen = dist(x1, y1, x2, y2);

	// since floats are so minutely accurate, add
	// a little buffer zone that will give collision
	float buffer = 0.1f;    // higher # = less accurate

	// if the two distances are equal to the line's
	// length, the point is on the line!
	// note we use the buffer here to give a range, rather
	// than one #
	if (d1 + d2 >= lineLen - buffer && d1 + d2 <= lineLen + buffer) {
		return true;
	}
	return false;
}

// POINT/CIRCLE Collision
bool pointCircle(float px, float py, float cx, float cy, float r)
{
	// get distance between the point and circle's center
	// using the Pythagorean Theorem
	float distX = px - cx;
	float distY = py - cy;
	float distance = sqrt((distX * distX) + (distY * distY));

	// if the distance is less than the circle's 
	// radius the point is inside!
	if (distance <= r) {
		return true;
	}
	return false;
}

// LINE/CIRCLE Collision
bool lineCircle(float x1, float y1, float x2, float y2, float cx, float cy, float r) 
{
	// is either end INSIDE the circle?
	// if so, return true immediately
	bool inside1 = pointCircle(x1, y1, cx, cy, r);
	bool inside2 = pointCircle(x2, y2, cx, cy, r);
	if (inside1 || inside2) return true;

	// get length of the line
	float distX = x1 - x2;
	float distY = y1 - y2;
	float len = sqrt((distX * distX) + (distY * distY));

	// get dot product of the line and circle
	float dot = (((cx - x1) * (x2 - x1)) + ((cy - y1) * (y2 - y1))) / pow(len, 2);

	// find the closest point on the line
	float closestX = x1 + (dot * (x2 - x1));
	float closestY = y1 + (dot * (y2 - y1));

	// is this point actually on the line segment?
	// if so keep going, but if not, return false
	bool onSegment = linePoint(x1, y1, x2, y2, closestX, closestY);
	if (!onSegment) return false;

	// optionally, draw a circle at the closest point
	// on the line
	//fill(255, 0, 0);
	//noStroke();
	//ellipse(closestX, closestY, 20, 20);

	// get distance to closest point
	distX = closestX - cx;
	distY = closestY - cy;
	float distance = sqrt((distX * distX) + (distY * distY));

	// is the circle on the line?
	if (distance <= r)
	{
		return true;
	}
	return false;
}

