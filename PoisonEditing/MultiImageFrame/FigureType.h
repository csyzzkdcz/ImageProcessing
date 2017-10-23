#pragma once
#include<vector>
#include<qrect.h>
#include<qpoint.h>
#include "Eigen/Eigen"
#include "Eigen/StdVector"			// in order to use an aligned allocator
#define INF 1000000;
#define eps 1e-6
using namespace std;
using namespace Eigen;

class FigureType
{
public:
	FigureType();
	virtual ~FigureType();

public:
	virtual void Get_Vertices();
	virtual void Generate_BBox();
	virtual void Get_Points_Num();
	virtual void Get_Y_Max();
	virtual void Get_Y_Min();
	virtual void Get_X_Max();
	virtual void Get_X_Min();

public:
	//vector<Vector2d,Eigen::aligned_allocator<Vector2d>> m_vertices;	// the vertice of the chosen area
	vector<QPointF> m_vertices;				// change the points from int to float
	int y_max_,y_min_,x_max_,x_min_;
	QRect BBox_;
	int points_num_;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};