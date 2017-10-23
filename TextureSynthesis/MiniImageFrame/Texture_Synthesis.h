#pragma once
#include"PlacePatch.h"
#include"PlacePatch_EPM.h"
#include"PlacePatch_RPM.h"
#include"PlacePatch_SPM.h"
#include"graph.h"

typedef Graph<float, float, float> GraphType;
struct SEAM_NODE
{
	int flag ;	// judge whether this seam nodes works, 0 means invalid, 1 means valid
	int nodex;  // record the index of this node in the graph
	// the rgb value of the start pixel currently, i.e. As(s)
	int As_Sr;
	int As_Sb;
	int As_Sg;

	// the rgb value of the start pixel currently, i.e. At(s)
	int At_Sr;
	int At_Sb;
	int At_Sg;

	// the rgb value of the end pixle currently, i.e As(t)
	int As_Tr;
	int As_Tb;
	int As_Tg;

	// the rgb value of the end pixle currently, i.e. At(t)
	int At_Tr;
	int At_Tb;
	int At_Tg;
};

class Synthesis
{
public:
	Synthesis(Mat input_text);
	Synthesis();
	virtual ~Synthesis();

public:
	Mat Do_Synthesis(Methods method_type, Mat input_text, Mat &seams_, double extended_ratio_x, double extended_ratio_y);

private:
	void Initialize();
	void Graph_Generate(GraphType* G, vector<int> pos);	//generate the graph based on the overlapping pixels
	void Graph_Cut_Process(GraphType* G, vector<int> pos);	//cutting the graph using Max-flow theory
	bool Is_Belong_SOURCE(int i, int j);			
	/* judge whether a pixel(i,j) need to connect with SOURCE(patch)
	(i,j) is the coordinate of the pixel corresponding to the output image.
	We have assume that the pixel(i,j) is the overlapping point
	*/
	bool Is_Belong_SINK(vector<int> pos, int i, int j);
	/* judge whether a pixel(i,j) need to connect with SINK(output image)
	pos is the translation of the patch;
	(i,j) is the coordinate of the pixel corresponding to the output image.
	We have assume that the pixel(i,j) is the overlapping point
	*/
	float Edge_Weight(int Out_Sr, int Out_Sb, int Out_Sg, int in_Sr, int in_Sb, int in_Sg, 
					 int Out_Er, int Out_Eb, int Out_Eg, int in_Er, int in_Eb, int in_Eg);

private:
	Place_Patch*  place_method_;
	Mat input_texture_;
	Mat output_image_;
	int** image_mask_;				/*it has two functions: 
										1: record whether the pixel is colored(!=0)
										2: record the node index in the overlapping graph
										(when pixel(i,j) is initially colored, we setimage_mask_(i,j)=1)
									*/
	float** seams_mark_v_;			// Record whether output image has a seam at position(i,j) from vertical direction
	float** seams_mark_h_;			// Record whether output image has a seam at position(i,j) from horizontal direction
	int txt_width_;
	int txt_height_;
	int pat_width_;
	int pat_height_;
	int total_pixels_;
	int colored_pixels_;
	vector<SEAM_NODE> seam_nodes_v_; // vertical seam nodes
	vector<SEAM_NODE> seam_nodes_h_; // horizontal seam nodes
	vector < GraphType::node_id > nodes;
};