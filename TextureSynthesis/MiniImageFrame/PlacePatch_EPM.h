#pragma once
#include"PlacePatch.h"

class Place_Patch_EPM : public Place_Patch
{
public:
	Place_Patch_EPM();
	~Place_Patch_EPM();
	void Placing_Method(Mat output_im, Mat input_im, int** image_mask,
		float** seams_mark_h_, float** seams_mark_v_, vector<int> &pos, bool is_finished_);
};