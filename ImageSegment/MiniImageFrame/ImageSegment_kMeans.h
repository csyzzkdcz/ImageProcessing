#pragma once
#include"ImageSegment.h"
#ifndef CLASS_NUM_
#define CLASS_NUM_ 10
#endif
class ImageSegment_kMeans : public ImageSegment
{
public:
	ImageSegment_kMeans();
	~ImageSegment_kMeans();
	ImageSegment_kMeans(Mat image_mat_);
	void DoSegment(Mat &ptr_image_);

private:
	void Calculate_Mean(vector<int> class_coord, Vector3d &mean);
	void Calculate_Var(vector<int> class_coord, int class_pos_, double &var);
	void Classify();
	bool Is_Finished();
	double Get_Distance(double x1, double y1, double z1, double x2, double y2, double z2);

private:
	Mat mat_backup_;
	int mat_cols_;
	int mat_rows_;
	vector<Vector3d> class_centers_;
};