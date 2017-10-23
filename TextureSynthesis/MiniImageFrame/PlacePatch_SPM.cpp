#include"PlacePatch_SPM.h"
Place_Patch_SPM::Place_Patch_SPM()
{

}

Place_Patch_SPM::~Place_Patch_SPM()
{

}

void Place_Patch_SPM::Placing_Method(Mat output_im, Mat input_im, int** image_mask,
	float** seams_mark_h_, float** seams_mark_v_, vector<int> &pos, bool is_finished_)
{
	pat_width_ = input_im.cols;
	pat_height_ = input_im.rows;
	txt_width_ = output_im.cols;
	txt_height_ = output_im.rows;
	int select_area_width_ =int(pat_width_ *  ERROR_REGION_RATIO);
	int select_area_height_ =int(pat_height_ * ERROR_REGION_RATIO);
	int count_, pos_x_, pos_y_;
	int k;
	int transition_x_in_=0, transition_y_in_=0;
	int temp_cost_,min_cost_=INF_COST;
	int best_x_ = -1, best_y_ = -1;
	if (!is_finished_)
		// whilw un-finished, the error region is the the region have enough overlaps as well as empty
	{
		// Find the subpatch, which is slightly larger than the error region
		for (;;)
		{
			count_ = 0;
			//srand((unsigned)time(NULL));
			// Set the random position
			pos_x_ = rand() % txt_width_;
			pos_y_ = rand() % txt_height_;
			for (int i = 0; i < select_area_width_; i++)
			for (int j = 0; j < select_area_height_; j++)
			{
				if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
					count_++;
			}
			// if the count is bigger than given bound, we think that this patch is what we need
			if (count_ > RATIO * select_area_width_*select_area_height_ && count_ < select_area_width_*select_area_height_)
				break;
		}
		int temp_x_, temp_y_, min_x_, max_x_,min_y_,max_y_;
		min_x_ = int(pos_x_ - (1 - ERROR_REGION_RATIO)*pat_width_)>0 ? int(pos_x_ - (1 - ERROR_REGION_RATIO)*pat_width_) : 0;
		max_x_ = pos_x_;
		min_y_ = int(pos_y_ - (1 - ERROR_REGION_RATIO)*pat_width_)>0 ? int(pos_y_ - (1 - ERROR_REGION_RATIO)*pat_width_) : 0;
		max_y_ = pos_y_;
		best_x_ = pos_x_;
		best_y_ = pos_y_;
		// Find the best matched region in the input image
		for (int i = 0; i < PLACE_TEST_NUM; i++)
		{
			temp_cost_ = 0;
			if (max_x_ - min_x_ == 0)
				temp_x_ = min_x_;
			else
				temp_x_ = rand() % (max_x_ - min_x_) + min_x_;
			if (max_y_ - min_y_ == 0)
				temp_y_ = min_y_;
			else
				temp_y_ = rand() % (max_y_ - min_y_) + min_y_;
			for (int i = 0; i < select_area_width_; i++)
			for (int j = 0; j < select_area_height_; j++)
			{
				if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
				for (int p = 0; p < 3; p++)
					temp_cost_ += pow(abs(input_im.at<cv::Vec3b>(pos_y_-temp_y_, pos_x_-temp_x_)(p)-output_im.at<cv::Vec3b>((j + pos_y_) % txt_height_, (i + pos_x_) % txt_width_)(p)), 2.0);
			}
			temp_cost_ = temp_cost_ / count_;
			if (temp_cost_ < min_cost_)
			{
				min_cost_ = temp_cost_;
				best_x_ = temp_x_;
				best_y_ = temp_y_;
				transition_x_in_ = pos_x_ - best_x_;
				transition_y_in_ = pos_y_ - best_y_;
			}

		}
	}
	else
		// Refine 
	{
		count_ = 0;
		// We choose the error region as sub-patch of the output image
		Find_Error_Region(seams_mark_h_, seams_mark_v_, pos_x_, pos_y_);
		int temp_x_, temp_y_, min_x_, max_x_, min_y_, max_y_;
		min_x_ = int(pos_x_ - (1 - ERROR_REGION_RATIO)*pat_width_)>0 ? int(pos_x_ - (1 - ERROR_REGION_RATIO)*pat_width_) : 0;
		max_x_ = pos_x_;
		min_y_ = int(pos_y_ - (1 - ERROR_REGION_RATIO)*pat_width_)>0 ? int(pos_y_ - (1 - ERROR_REGION_RATIO)*pat_width_) : 0;
		max_y_ = pos_y_;
		best_x_ = pos_x_;
		best_y_ = pos_y_;
		// Find the best matched region in the input image
		for (int i = 0; i < PLACE_TEST_NUM; i++)
		{
			temp_cost_ = 0;
			if (max_x_ - min_x_ == 0)
				temp_x_ = min_x_;
			else
				temp_x_ = rand() % (max_x_ - min_x_) + min_x_;
			if (max_y_ - min_y_ == 0)
				temp_y_ = min_y_;
			else
				temp_y_ = rand() % (max_y_ - min_y_) + min_y_;
			for (int i = 0; i < select_area_width_; i++)
			for (int j = 0; j < select_area_height_; j++)
			{
				if (image_mask[(pos_y_ + j) % txt_height_][(pos_x_ + i) % txt_width_]>0)
				{
					for (int p = 0; p < 3; p++)
						temp_cost_ += pow(abs(input_im.at<cv::Vec3b>(pos_y_ - temp_y_, pos_x_ - temp_x_)(p)-output_im.at<cv::Vec3b>((j + pos_y_) % txt_height_, (i + pos_x_) % txt_width_)(p)), 2.0);
					count_++;
				}
			}
			temp_cost_ = temp_cost_ / count_;
			if (temp_cost_ < min_cost_)
			{
				min_cost_ = temp_cost_;
				best_x_ = temp_x_;
				best_y_ = temp_y_;
				transition_x_in_ = pos_x_ - best_x_;
				transition_y_in_ = pos_y_ - best_y_;
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
	pos.push_back(transition_x_in_);
	pos.push_back(select_area_width_);
	pos.push_back(transition_y_in_);
	pos.push_back(select_area_height_);
}
