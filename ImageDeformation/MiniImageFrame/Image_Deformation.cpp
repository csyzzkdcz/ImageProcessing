#include"Image_Deformation.h"
#define eps_ 1e-6
Image_Deformation::Image_Deformation()
{
	triangulation_method_ = new Triangulation();
}

Image_Deformation::~Image_Deformation()
{
	if(triangulation_method_)
		delete(triangulation_method_);
}

void Image_Deformation::Deformation(cv::Mat image_mat_,cv::Mat &target_image_mat_, PolygonCage* poly_cage_, PolygonCage* target_poly_cage_)
{
}

int Image_Deformation::Near_Int(double x)
{
	int  y = int(x);
	if(x-y>0.5)
		y=y+1;
	return y;
}
double Image_Deformation::dist(double point_start_x,double point_start_y,double point_end_x, double point_end_y) 
	// Calculate the distance from point_start_ to point_end_
{
	double dist_ = sqrt(pow(point_end_x-point_start_x,2.0)+ pow(point_end_y-point_start_y,2.0));
	return dist_ < eps_ ? eps_ : dist_;
}

double Image_Deformation::calculate_cosin(double P1_x,double P1_y,double P2_x,double P2_y,double P3_x,double P3_y)
	// calculate the cosin of <P3P1,P1P2>
{
	double dist_12_,dist_23_,dist_31_;
	dist_12_ = dist(P1_x,P1_y,P2_x,P2_y);
	dist_23_ = dist(P2_x,P2_y,P3_x,P3_y);
	dist_31_ = dist(P3_x,P3_y,P1_x,P1_y);
	if(dist_12_ <= eps_ || dist_31_ <= eps_)
		return eps_;
	if(dist_23_ <= eps_)
		return 1-eps_;
	double cos_ = (dist_31_ * dist_31_ + dist_12_ * dist_12_ - dist_23_ * dist_23_)/(2 * dist_12_ * dist_31_);
	if(0 < cos_ && cos_< eps_)
		cos_ = eps_;
	if(-eps_ < cos_ && cos_ < 0)
		cos_ = -eps_;
	if(abs(1-cos_) < eps_)
		cos_ =1-eps_;
	if(abs(1+cos_) < eps_)
		cos_ = -1+eps_;
	return cos_;
}

double Image_Deformation::calculate_half_tangent(double P1_x,double P1_y,double P2_x,double P2_y,double P3_x,double P3_y)
	 // calculate the tan of 0.5*<P3P1,P1P2>
{
	double cos_ = calculate_cosin(P1_x,P1_y,P2_x,P2_y,P3_x,P3_y);
	return sqrt( (1-cos_)/(1+cos_) );
}


double Image_Deformation::calculate_cotangent(double P1_x,double P1_y,double P2_x,double P2_y,double P3_x,double P3_y)
	// calculate the cotan of <P3P1,P1P2>
{
	double cos_ = calculate_cosin(P1_x,P1_y,P2_x,P2_y,P3_x,P3_y);
	return cos_/sqrt(1-cos_*cos_);
}
