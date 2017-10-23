#include"PlacePatch.h"
Place_Patch::Place_Patch()
{

}

Place_Patch::~Place_Patch()
{

}

void Place_Patch::Placing_Method(Mat output_im, Mat input_im, int** image_mask,
	float** seams_mark_h_, float** seams_mark_v_, vector<int> &pos, bool is_finished_)
{

}

void Place_Patch::Find_Error_Region(float** seams_mark_h_, float** seams_mark_v_, int& pos_x_, int& pos_y_)
{
	if (txt_width_ == 0)
	{
		pos_x_ = pos_y_ = -1;
		return;
	}
	int ER_Width_ = ERROR_REGION_RATIO*pat_width_;
	int ER_Height_ = ERROR_REGION_RATIO*pat_height_;
	float cost_ = -1;
	float temp_cost_=0;
	pos_x_ = pos_y_ = 0;
	int i, j;
	for (i = 0; i < txt_width_; i = i + int(ER_Width_ / 2))
	{
		for (j = 0; j < txt_height_; j = j + int(ER_Height_ / 2))
		{
			temp_cost_ = Calculate_Cost(i, j, seams_mark_h_, seams_mark_v_);
			if (temp_cost_ > cost_)
			{
				pos_x_ = i;
				pos_y_ = j;
				cost_ = temp_cost_;
			}
		}
	}
}

float Place_Patch::Calculate_Cost(int pos_x_, int pos_y_, float** seams_mark_h_, float** seams_mark_v_)
{
	float cost_ = 0;
	for (int i = 0; i < ERROR_REGION_RATIO*pat_width_; i++)
	for (int j = 0; j < ERROR_REGION_RATIO*pat_height_; j++)
	{
		cost_ = cost_ + seams_mark_h_[(j + pos_y_) % txt_height_][(i + pos_x_) % txt_width_] + seams_mark_v_[(j + pos_y_) % txt_height_][(i + pos_x_) % txt_width_];
	}
	return cost_;
}