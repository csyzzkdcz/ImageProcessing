#include"PlacePatch_RPM.h"
#ifndef MAX_ITR
#define MAX_ITR 10000
#endif
Place_Patch_RPM::Place_Patch_RPM()
{
	int pat_width_ = 0;
	int pat_height_ = 0;
	int txt_width_ = 0;
	int txt_height_ = 0;
}

Place_Patch_RPM::~Place_Patch_RPM()
{

}

void Place_Patch_RPM::Placing_Method(Mat output_im, Mat input_im, int** image_mask,
	float** seams_mark_h_, float** seams_mark_v_, vector<int> &pos, bool is_finished_)

/*Randomly select postion which covers the error region.
There we call a rigion the error region if the number of 
overlapping pixels is bigger than a given radio multiple 
the size of input image when initializing */

{
	pat_width_ = input_im.cols;
	pat_height_ = input_im.rows;
	txt_width_ = output_im.cols;
	txt_height_ = output_im.rows;
	int count_,pos_x_,pos_y_;
	int k;
	if (!is_finished_)
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
		/*if (k == MAX_ITR)
		{
			pos_x_ = -1;
			pos_y_ = -1;
		}*/
	}
	else
	{
		int temp_x_, temp_y_;
		Find_Error_Region(seams_mark_h_, seams_mark_v_, pos_x_, pos_y_);
		float center_p_x_ = pos_x_ + ERROR_REGION_RATIO * pat_width_/ 2.0;
		float center_p_y_ = pos_y_ + ERROR_REGION_RATIO * pat_height_ / 2.0;
		float max_x_ =(1 - ERROR_REGION_RATIO)*pat_width_;
		float max_y_ =(1 - ERROR_REGION_RATIO)*pat_height_;
		float center_x_ = -0.5*max_x_ + ((float)rand() / RAND_MAX)*max_x_+center_p_x_;
		float center_y_ = -0.5*max_y_ + ((float)rand() / RAND_MAX)*max_y_+center_p_y_;
		temp_x_ = int(center_x_ - 0.5*pat_width_);
		temp_y_ = int(center_y_ - 0.5*pat_height_);
		if (temp_x_ < txt_width_ && temp_x_ >= 0 && temp_y_ < txt_height_ && temp_y_ >= 0)
		{
			pos_x_ = temp_x_;
			pos_y_ = temp_y_;
		}
	}
	pos.clear();
	// the translation
	pos.push_back(pos_x_);
	pos.push_back(pos_y_);

	// #overlapping pixels
	pos.push_back(count_);

	// the position corresponding to the input image
	pos.push_back(0);
	pos.push_back(pat_width_);
	pos.push_back(0);
	pos.push_back(pat_height_);
}
