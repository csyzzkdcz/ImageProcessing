#include"ImageSegment_kMeans.h"
#ifndef MAX_EPS_
#define MAX_EPS_ 0.1
#endif
#ifndef TIMES_
#define TIMES_ 20
#endif
ImageSegment_kMeans::ImageSegment_kMeans()
{

}

ImageSegment_kMeans::~ImageSegment_kMeans()
{

}

ImageSegment_kMeans::ImageSegment_kMeans(Mat ptr_image_)
{
	if (ptr_image_.rows > ptr_image_.cols)
	{
		cout << "Rows# is more than Cols#" << endl;
		return;
	}
	Gaussian_Filter(ptr_image_);
	mat_backup_ = ptr_image_.clone();
	for (int i = 0; i < class_components_.size(); i++)
	{
		class_components_[i].clear();
	}
	class_centers_.resize(CLASS_NUM_);
	class_components_.resize(CLASS_NUM_);
	mat_cols_ = ptr_image_.cols;
	mat_rows_ = ptr_image_.rows;
}

void ImageSegment_kMeans::DoSegment(Mat &ptr_image_)
{
	int cols_ = ptr_image_.cols;
	int rows_ = ptr_image_.rows;
	int max_size_ = cols_ * rows_;
	for (int i = 0; i < CLASS_NUM_; i++)
	{
		int center_id_ = rand() % max_size_;
		int center_pos_x_ = center_id_ / cols_;
		int center_pos_y_ = center_id_ - center_pos_x_*cols_;
		class_centers_[i][0] = ptr_image_.at<Vec3b>(center_pos_x_, center_pos_y_)[0];
		class_centers_[i][1] = ptr_image_.at<Vec3b>(center_pos_x_, center_pos_y_)[1];
		class_centers_[i][2] = ptr_image_.at<Vec3b>(center_pos_x_, center_pos_y_)[2];
	}
	Classify();
	int times_=int(log(mat_cols_*mat_rows_*1.0));
	int itr_ = 0;
	while (!Is_Finished() && itr_ < TIMES_ && itr_ < times_)
	{
		for (int i = 0; i < CLASS_NUM_; i++)
			Calculate_Mean(class_components_[i], class_centers_[i]);
		Classify();
		itr_++;
	}
	Output_Image(ptr_image_);
}

void ImageSegment_kMeans::Classify()
{
	for (int i = 0; i < CLASS_NUM_; i++)
	{
		class_components_[i].clear();
	}
	for (int i = 0; i < mat_rows_;i++)
	for (int j = 0; j < mat_cols_; j++)
	{
		double min_ = Get_Distance(double(mat_backup_.at<Vec3b>(i, j)[0]),double(mat_backup_.at<Vec3b>(i, j)[1]), double(mat_backup_.at<Vec3b>(i, j)[2]), class_centers_[0][0], class_centers_[0][1], class_centers_[0][2]);
		int min_coord_ = 0;
		for (int k = 1; k < CLASS_NUM_; k++)
		{
			if (min_ > Get_Distance(double(mat_backup_.at<Vec3b>(i, j)[0]), double(mat_backup_.at<Vec3b>(i, j)[1]), double(mat_backup_.at<Vec3b>(i, j)[2]), class_centers_[k][0], class_centers_[k][1], class_centers_[k][2]))
				min_coord_ = k;
		}
		class_components_[min_coord_].push_back(i*mat_cols_ + j);
	}
}

bool ImageSegment_kMeans::Is_Finished()
{
	bool is_finish_ = true;
	double var_;
	for (int i = 0; i < CLASS_NUM_; i++)
	{
		Calculate_Var(class_components_[i], i, var_);
		if (var_ > MAX_EPS_)
			is_finish_ = false;
	}
	return is_finish_;
}

void ImageSegment_kMeans::Calculate_Mean(vector<int> class_coord, Vector3d &mean)
{
	mean(0) = mean(1) = mean(2) = 0;
	int i, size_ = class_coord.size();
	int cols_ = mat_backup_.cols;
	for (i = 0; i < size_; i++)
	{
		int pos_x_ = class_coord[i] / cols_;
		int pos_y_ = class_coord[i] - cols_*pos_x_;
		mean(0) += mat_backup_.at<Vec3b>(pos_x_, pos_y_)[0];
		mean(1) += mat_backup_.at<Vec3b>(pos_x_, pos_y_)[1];
		mean(2) += mat_backup_.at<Vec3b>(pos_x_, pos_y_)[2];
	}
	mean(0) = mean(0) / size_;
	mean(1) = mean(1) / size_;
	mean(2) = mean(2) / size_;

}

void ImageSegment_kMeans::Calculate_Var(vector<int> class_coord, int class_pos_, double &var)
{
	double mean_x_ = class_centers_[class_pos_][0];
	double mean_y_ = class_centers_[class_pos_][1];
	double mean_z_ = class_centers_[class_pos_][2];
	double var_x_, var_y_, var_z_;
	var_x_ = var_y_ = var_z_ = 0;
	int i, size_ = class_coord.size();
	int cols_ = mat_backup_.cols;
	for (i = 0; i < size_; i++)
	{
		int pos_x_ = class_coord[i] / cols_;
		int pos_y_ = class_coord[i] - cols_*pos_x_;
		var_x_ += pow(mean_x_ - mat_backup_.at<Vec3b>(pos_x_, pos_y_)[0], 2);
		var_y_ += pow(mean_y_ - mat_backup_.at<Vec3b>(pos_x_, pos_y_)[1], 2);
		var_z_ += pow(mean_z_ - mat_backup_.at<Vec3b>(pos_x_, pos_y_)[2], 2);
	}
	var = sqrt(var_x_ + var_y_ + var_z_);
}

double ImageSegment_kMeans::Get_Distance(double x1,double y1, double z1, double x2, double y2,double z2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}