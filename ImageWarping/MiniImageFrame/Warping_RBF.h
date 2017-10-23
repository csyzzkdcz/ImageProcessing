#pragma once
#include"Warping.h"


class Warping_RBF : public Warping
{
public:
	Warping_RBF();
	~Warping_RBF();
	void DoWarping_(QImage &image_);

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	void Calculate_Affine_Component_();			// The affine component Ax = Mx + b		
	void Calculate_Radial_Basis_Fun_();			// Calculate the coefficients of the radial basis function

private:
	double			r;							// the parameter of radial basis function
	Matrix2d		M;
	Vector2d		b;							// the affine component is Ax = Mx+b
	VectorXd		alpha_x_;					// the coefficient vector
	VectorXd		alpha_y_;					// the coefficient vector
	vector<double>		dist_min_r_;				// the vector to store the minimine distance between Pi and Pj for all j!=i
};