#pragma once
#include<vector>
#include<qpoint.h>
#include<qrect.h>
using namespace std;
class PolygonCage
{
public:
	PolygonCage();
	PolygonCage(vector<QPoint> vertices_);
	~PolygonCage();

private:
	void Get_Y_Max();
	void Get_Y_Min();
	void Get_X_Max();
	void Get_X_Min();

public:
	QRect					BBox_;
	int						x_min_;
	int						y_min_;
	int						x_max_;
	int						y_max_;
	int						points_num_;
	vector<QPointF>			vertices_;

};