#pragma once
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <time.h>
using namespace std;
using namespace cv;

#ifndef TEST_NUM
#define TEST_NUM 100
#endif

#ifndef RATIO
#define RATIO 0.1
#endif

#ifndef ERROR_REGION_RATIO
#define ERROR_REGION_RATIO 0.5
#endif

#ifndef PLACE_TEST_NUM
#define PLACE_TEST_NUM 100
#endif


#ifndef INF_COST
#define INF_COST 1e5
#endif

enum Methods
{
	kRandom,
	kEntireMatch,
	kSubPatch,
	kDefault
};

class Place_Patch
{
public:
	Place_Patch();
	virtual ~Place_Patch();
	virtual void Placing_Method(Mat output_im, Mat input_im, int** image_mask,
		float** seams_mark_h_,float** seams_mark_v_,vector<int> &pos,bool is_finished_);
	/* 
	This function aims to find the proper position to place the patch:
	image_mask_ is used to record whether a pixel has been colored; 
	seams_mark_h_ & seams_mark_v_ are used to record the cost of each seam;
	which means that there is no pixel uncolored;
	is_finished_ is the flag to judge whether initialization is finished;
	pos record the coordinate of the left-top corner of the patch.
	More specifically, pos[0] = left-top.rx(in the output_im)
					  pos[1] = left-top.ry(in the output_im)
					  pos[2] = #overlapping pixels
					  pos[3] = left-top.rx(in input_im)
					  pos[4] = left-top.ry(in input_im)
					  pos[5] = right-down.rx(in input_im)
					  pos[6] = right-down.ry(in input_im)
	*/

protected:
	void Find_Error_Region(float** seams_mark_h_, float** seams_mark_v_, int &pos_x_, int &pos_y_);
	float Calculate_Cost(int pos_x, int pos_y_, float** seams_mark_h_, float** seams_mark_v_);

protected:
	int pat_width_;
	int pat_height_;
	int txt_width_;
	int txt_height_;
};
