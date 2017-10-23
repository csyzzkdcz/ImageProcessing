#pragma once
#include"cmath"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "Triangulation.h"

class Image_Deformation
{
public:
	Image_Deformation();
	virtual ~Image_Deformation();
	virtual void Deformation(cv::Mat image_mat_,cv::Mat &target_image_mat_,PolygonCage *poly_cage_, PolygonCage *target_poly_cage_);

protected:
	double dist(double point_start_x,double point_start_y,double point_end_x,double point_end_y);	// calculate the dist from start point to end point
	double calculate_cosin(double point1_x,double point1_y, double point2_x,double point2_y, double point3_x,double point3_y); // calculate the cosin of <P3P1,P1P2>
	double calculate_half_tangent(double point1_x,double point1_y, double point2_x,double point2_y, double point3_x,double point3_y); // calculate the tan of 0.5*<P3P1,P1P2>
	double calculate_cotangent(double point1_x,double point1_y, double point2_x,double point2_y, double point3_x,double point3_y); // calculate the cotangent of <P3P1,P1P2>

	int Near_Int(double x);											// find the nearest integer for x

protected:
	struct triangulateio		triangle_frame_;
	Triangulation				*triangulation_method_;
	
};