#pragma once
#include"Image_Deformation.h"

class Image_Deformation_WP : public Image_Deformation
{
public: 
	Image_Deformation_WP();
	~Image_Deformation_WP();
	void Deformation(cv::Mat image_mat_,cv::Mat &target_image_mat_,PolygonCage *poly_cage_, PolygonCage *target_poly_cage_);
};