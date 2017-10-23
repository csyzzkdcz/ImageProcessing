#pragma once
#include"Image_Deformation.h"

class Image_Deformation_MV : public Image_Deformation
{
public: 
	Image_Deformation_MV();
	~Image_Deformation_MV();
	void Deformation(cv::Mat image_mat_,cv::Mat &target_image_mat_,PolygonCage *poly_cage_, PolygonCage *target_poly_cage_);
};