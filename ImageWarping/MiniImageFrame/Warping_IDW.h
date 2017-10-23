#pragma once
#include"Warping.h"
#include<Eigen>
#define DIST_MIN 0.00001
#define RADUIS 10000
using namespace Eigen;
class Warping_IDW : public Warping
{
public:
	Warping_IDW();
	~Warping_IDW();
	void DoWarping_(QImage& image_);

private:
	double calculate_sigma_(QPointF point_, QPointF base_point_);
	void Calculate_Coef_Matrix_(int i, Vector4d &T);		//Calculate the Matrix Ti

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};