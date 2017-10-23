#pragma once
#include"FigureType.h"
#include<qpoint.h>
#include<vector>
#include<qpainter.h>
using namespace std;

class FigureTypeRect : public FigureType
{
public:
	FigureTypeRect();
	~FigureTypeRect();
	FigureTypeRect(QPoint start_point_, QPoint end_point_);

public:
	int Get_Width_();
	int Get_Height_();
	QPoint Get_Left_Button_();
	QPoint Get_Right_Up_();
	bool Is_Boundary(int i,int j);

public:
	void Get_Vertices();
	void Generate_BBox();
	void Get_Y_Max();
	void Get_Y_Min();
	void Get_X_Max();
	void Get_X_Min();
	void Get_Points_Num();
	void Draw_Selected_Region_(QPainter &paint);

public:
	vector<QPointF> Boundary_Points;

private:
	int width;
	int height;
	QPoint Left_Bottom;
	QPoint Right_Up;
	QPoint point_start_;
	QPoint point_end_;
};