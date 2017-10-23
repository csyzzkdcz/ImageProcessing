#pragma once
#include"FigureType.h"
#include<qpoint.h>
#include<vector>
#include<qpainter.h>
using namespace std;

class FigureTypePolygon : public FigureType
{
public:
	FigureTypePolygon(vector<QPoint> points_array_);
	~FigureTypePolygon();

public:
	void Draw_Selected_Region_(QPainter &paint);
	void Get_Vertices();
	void Generate_BBox();
	void Get_Y_Max();
	void Get_Y_Min();
	void Get_X_Max();
	void Get_X_Min();
	void Get_Points_Num();

private:
	vector<QPoint> Points_;
};