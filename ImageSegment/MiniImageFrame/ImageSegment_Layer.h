#pragma once
#include"ImageSegment.h"
#ifndef ADJACENT_NUM_
#define ADJACENT_NUM_ 9
#endif

class ImageSegment_Layer : public ImageSegment
{
public:
	ImageSegment_Layer();
	~ImageSegment_Layer();
	ImageSegment_Layer(Mat image_mat_);
	void DoSegment(Mat &ptr_image_);

private:
	void Generate_kdTree(Mat image_mat_);

	void  Find_Adjacent_Points_ANN_(vector<double > feature_pos_, int num_, int width_, int height_, int pos_x_, int pos_y_);
											// Find the neerest points with the number of k of a point
	void Init(Mat &ptr_image_);				// Initialize the iamge to get the k-nearest neighbor
	double Min(double a, double b);
	void Update_ClassComponents(int id_ix_,int id_iy_, int id_jx_, int id_jy_, int edge_id_);

private:
	ANNkd_tree*			kdTree;
	vector<QPointF> Adjacent_Points_;
	vector<double> Adjacent_dist_;
	vector<EDGE> edges_;
	MatrixXi  edges_mat_;
	MatrixXi id_mat_;
	VectorXi id_sum_;
	VectorXd id_int_;
};

