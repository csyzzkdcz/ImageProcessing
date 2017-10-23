#pragma once
#include<vector>
#include <ANN/ANN.h>					// ANN declarations
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include<qpoint.h>
#include <Eigen>
using namespace Eigen;


struct EDGE
{
	int index_ix_;
	int index_iy_;
	int index_jx_;
	int index_jy_;
	double weight_;
};

using namespace std;
using namespace cv;
class ImageSegment
{
public:
	ImageSegment();
	virtual ~ImageSegment() = 0;
	void virtual DoSegment(Mat &ptr_image_);

protected:
	void Gaussian_Filter(Mat &ptr_image_);		// Gaussian filter
	int Find_Nearest_Int_(double x);			// Find the nearest integer for x
	void Output_Image(Mat &ptr_image_);

protected:
	vector<vector<int> > class_components_;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};