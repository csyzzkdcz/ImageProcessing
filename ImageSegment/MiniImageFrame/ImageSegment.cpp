#include"ImageSegment.h"
#include<algorithm>
#include <functional>
#include<iostream>
using namespace std;


ImageSegment::ImageSegment()
{
	
}

ImageSegment::~ImageSegment()
{

}

void ImageSegment::DoSegment(Mat &ptr_iamge_)
{

}

int ImageSegment::Find_Nearest_Int_(double x)
{
	return x - int(x) > 0.5 ? int(x) + 1 : int(x);
}

void ImageSegment::Gaussian_Filter(Mat &ptr_image_)
{
	Mat filter_mat_;
	GaussianBlur(ptr_image_, filter_mat_, Size(5, 5), 0.5, 0.5);
	ptr_image_ = filter_mat_.clone();
}

void ImageSegment::Output_Image(Mat &ptr_image_)
{
	int i, size_ = class_components_.size();
	for (i = 0; i < size_; i++)
	{
		if (class_components_[i].size())
		{
			int red_ = rand() / 256;
			int green_ = rand() / 256;
			int blue_ = rand() / 256;
			for (int j = 0; j < class_components_[i].size(); j++)
			{
				int pos_ = class_components_[i].at(j);
				int rx_ = pos_ / ptr_image_.cols;
				int ry_ = pos_ - rx_ * ptr_image_.cols;
				ptr_image_.at<Vec3b>(rx_, ry_)[0] = red_;
				ptr_image_.at<Vec3b>(rx_, ry_)[1] = green_;
				ptr_image_.at<Vec3b>(rx_, ry_)[2] = blue_;
			}
		}
	}
}