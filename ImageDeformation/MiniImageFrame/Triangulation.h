#pragma once
#include <qpoint.h>
#include "Polygon.h"
#include "ScanLine.h"
extern "C"{
#include"triangle.h"
};
#define MAX_PTS_NUM_ 20

class Triangulation
{
public:
	Triangulation();
	~Triangulation();

public:
	void Triangulation_with_rand(PolygonCage *poly_cage_, struct triangulateio &triangle_frame_);												// Triangulating the polygon cage with random distributed vertices
	void Triangulation_with_area_constraint(PolygonCage *poly_cage_, double Min_area_, double Max_area_,struct triangulateio &triangle_frame_);	// Triangulating the polygon cage with area constraints 
	void Triangulation_with_angle_constraint(PolygonCage *poly_cage_,struct triangulateio &triangle_frame_);									// Triangulating the polygon cage with area constraints, here we set the minimum angle is 20 degree

private:
	void generate_inner_points(PolygonCage *poly_cage_);			//generate inner points for triangliztion

private:
	ScanLine				*scan_inner_;
	vector<QPointF>			vertices_;								// the vertices of the polygon cage
	vector<QPointF>			inner_points_;							// the vector to record inner points
};