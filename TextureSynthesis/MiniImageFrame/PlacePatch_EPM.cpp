#include"PlacePatch_EPM.h"

Place_Patch_EPM::Place_Patch_EPM()
{

}

Place_Patch_EPM::~Place_Patch_EPM()
{

}

void Place_Patch_EPM::Placing_Method(Mat output_im, Mat input_im, int** image_mask,
	float** seams_mark_h_, float** seams_mark_v_, vector<int> &pos, bool is_finished_)
{
	pat_width_ = input_im.cols;
	pat_height_ = input_im.rows;
	txt_width_ = output_im.cols;
	txt_height_ = output_im.rows;
	int count_, pos_x_, pos_y_, best_x_=-1, best_y_=-1;
	int temp_cost_, min_cost_ = INF_COST;
	if (!is_finished_)
	{
		for (int test_ = 0; test_ < PLACE_TEST_NUM; test_++)
		{
			for (;;)
			{
				count_ = 0;
				//srand((unsigned)time(NULL));
				// Set the random position
				pos_x_ = rand() % txt_width_;
				pos_y_ = rand() % txt_height_;
				for (int i = 0; i < pat_width_; i++)
				for (int j = 0; j < pat_height_; j++)
				{
					if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
						count_++;
				}
				// if the count is bigger than given bound, we think that this patch is what we need
				if (count_ > RATIO * pat_height_*pat_width_ && count_ < pat_height_*pat_width_)
					break;
			}
			temp_cost_ = 0;
			for (int i = 0; i < pat_width_;i++)
			for (int j = 0; j < pat_height_; j++)
			{
				if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
				for (int p = 0; p < 3;p++)
					temp_cost_ += pow(abs(input_im.at<cv::Vec3b>(j, i)(p) - output_im.at<cv::Vec3b>((j + pos_y_) % txt_height_, (i + pos_x_) % txt_width_)(p)), 2.0);
			}
			temp_cost_ = temp_cost_ / count_;
			if (temp_cost_ < min_cost_)
			{
				min_cost_ = temp_cost_;
				best_x_ = pos_x_;
				best_y_ = pos_y_;
				
			}
		}
	}
	else
	{
		int temp_x_, temp_y_;
		Find_Error_Region(seams_mark_h_, seams_mark_v_, temp_x_, temp_y_);
		float center_p_x_ = temp_x_ + ERROR_REGION_RATIO * pat_width_ / 2.0;
		float center_p_y_ = temp_y_ + ERROR_REGION_RATIO * pat_height_ / 2.0;
		int max_x_ = int(0.5*(1 - ERROR_REGION_RATIO)*pat_width_);
		int max_y_ = int(0.5*(1 - ERROR_REGION_RATIO)*pat_height_);
		temp_cost_ = 0;
		for (int k = int(temp_x_ - (1 - ERROR_REGION_RATIO)*pat_width_); k <= temp_x_; k++)
		for (int l = int(temp_y_ - (1 - ERROR_REGION_RATIO)*pat_height_); l <= temp_x_; l++)
		{
			pos_x_ = (k + txt_width_) % txt_width_;
			pos_y_ = (l + txt_height_) % txt_height_;
			count_ = 0;
			for (int i = 0; i < pat_width_; i++)
			for (int j = 0; j < pat_height_; j++)
			{
				if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
				{
					for (int p = 0; p < 3; p++)
						temp_cost_ += pow(abs(input_im.at<cv::Vec3b>(j, i)(p)-output_im.at<cv::Vec3b>((j + pos_y_) % txt_height_, (i + pos_x_) % txt_width_)(p)), 2.0);
					count_++;
				}
			}
			temp_cost_ = temp_cost_ / count_;
			if (temp_cost_ < min_cost_)
			{
				min_cost_ = temp_cost_;
				best_x_ = pos_x_;
				best_y_ = pos_y_;

			}
		}
	}
	pos.clear();
	// the translation
	pos.push_back(best_x_);
	pos.push_back(best_y_);

	// #overlapping pixels
	pos.push_back(count_);

	// the position corresponding to the input image
	pos.push_back(0);
	pos.push_back(pat_width_);
	pos.push_back(0);
	pos.push_back(pat_height_);

}
