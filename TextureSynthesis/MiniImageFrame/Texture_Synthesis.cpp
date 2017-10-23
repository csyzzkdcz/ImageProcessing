#include"Texture_Synthesis.h"
#include<iostream>
using namespace std;
#ifndef INF
#define INF 100000
#endif

#ifndef EPS
#define EPS 1e-6
#endif

#ifndef ITR_NUM
#define ITR_NUM 5
#endif

Synthesis::Synthesis()
{
	pat_width_ = 0;
	pat_height_ = 0;
	txt_width_ = 0;
	txt_height_ = 0;
	colored_pixels_ = 0;
}

Synthesis::Synthesis(Mat input_text)
{
	input_texture_ = input_text.clone();
	txt_width_ = 0;
	txt_height_ = 0;
}

Synthesis::~Synthesis()
{
	if (place_method_)
		delete(place_method_);
	place_method_ = NULL;
}

void Synthesis::Initialize()
{
	if (!txt_width_ || !pat_width_)
		return;
	colored_pixels_ = 0;
	int pos_x_ = rand()%(txt_width_-pat_width_);
	int pos_y_ = rand()%(txt_height_-pat_height_);
	image_mask_ = new int*[txt_height_];
	seams_mark_v_ = new float*[txt_height_];
	seams_mark_h_ = new float*[txt_height_];
	for (int i = 0; i < txt_height_; i++)
	{
		image_mask_[i] = new int[txt_width_];
		seams_mark_v_[i] = new float[txt_width_];
		seams_mark_h_[i] = new float[txt_width_];
	}
	for (int i = 0; i < txt_height_;i++)
	for (int j = 0; j < txt_width_; j++)
	{
		image_mask_[i][j] = 0;
		seams_mark_v_[i][j] = 0;
		seams_mark_h_[i][j] = 0;
		output_image_.at<cv::Vec3b>(i, j)(0) = 0;
		output_image_.at<cv::Vec3b>(i, j)(1) = 0;
		output_image_.at<cv::Vec3b>(i, j)(2) = 0;
		
	}
	for (int i = pos_y_; i < pos_y_ + pat_height_;i++)
	for (int j = pos_x_; j < pos_x_ + pat_width_; j++)
	{
		output_image_.at<cv::Vec3b>(i, j)(0) = input_texture_.at<cv::Vec3b>(i - pos_y_, j - pos_x_)(0);
		output_image_.at<cv::Vec3b>(i, j)(1) = input_texture_.at<cv::Vec3b>(i - pos_y_, j - pos_x_)(1);
		output_image_.at<cv::Vec3b>(i, j)(2) = input_texture_.at<cv::Vec3b>(i - pos_y_, j - pos_x_)(2);
		colored_pixels_++;
		image_mask_[i][j] = INF;
	}

	// initialize the seam nodes
	seam_nodes_v_.clear();
	seam_nodes_h_.clear();

	for (int i = 0; i < txt_height_*txt_width_; i++)
	{
		SEAM_NODE node_;
		node_.flag = 0;
		node_.nodex = -1;
		seam_nodes_h_.push_back(node_);
		seam_nodes_v_.push_back(node_);
	}

}

Mat Synthesis::Do_Synthesis(Methods method_type, Mat input_text, Mat &seams, double extended_ratio_x, double extended_ratio_y)
{
	bool is_finished_ = false;
	input_texture_ = input_text.clone();
	pat_height_ = input_text.rows;
	pat_width_ = input_text.cols;
	txt_height_ = int(extended_ratio_y*pat_height_);
	txt_width_ = int(extended_ratio_x*pat_width_);
	total_pixels_ = txt_height_*txt_width_;
	output_image_ = Mat::zeros(txt_height_,txt_width_ ,CV_8UC3);
	Initialize();
	switch (method_type)
	{
	case kRandom:
		place_method_ = new Place_Patch_RPM();
		break;
	case kEntireMatch:
		place_method_ = new Place_Patch_EPM();
		break;
	case kSubPatch:
		place_method_ = new Place_Patch_SPM();
		break;
	case kDefault:
		break;
	default:
		break;
	}
	int flag_ = 0;
	while (colored_pixels_ <  total_pixels_)
	{
		// initialize the graph with #vertices=pat_height_*pat_width_, #edges=pat_height_*pat_width_
		GraphType* G = new GraphType(pat_height_*pat_width_, pat_height_*pat_width_); 
		
		vector<int> pos_;
		//cout << 1 << endl;
		place_method_->Placing_Method(output_image_, input_texture_, image_mask_, seams_mark_h_,seams_mark_v_ ,pos_, is_finished_);
		if (pos_[1] == -1)
			break;
		//cout << 2 << endl;
		Graph_Generate(G, pos_);
		//cout << 3 << endl;
		// Computes maxflow 
		G->maxflow();
		//cout << 4 << endl;
		// Copy the cut pixels
		Graph_Cut_Process(G, pos_);
		flag_++;
		//if (flag_==1)
		//	break;
		cout << flag_ << endl;
	}

	is_finished_ = true;
	// Refine the result£¬As for EPM, you'd better omit refine process for it is very slow!!!!
	for (int itr_ = 0; itr_ < ITR_NUM && colored_pixels_ == total_pixels_; itr_++)
	{
		// initialize the graph with #vertices=pat_height_*pat_width_, #edges=pat_height_*pat_width_
		GraphType* G = new GraphType(pat_height_*pat_width_, pat_height_*pat_width_);

		vector<int> pos_;
		
		place_method_->Placing_Method(output_image_, input_texture_, image_mask_, seams_mark_h_, seams_mark_v_, pos_, is_finished_);
		if (pos_[1] == -1)
			break;

		Graph_Generate(G, pos_);
		
		// Computes maxflow 
		G->maxflow();
		
		// Copy the cut pixels
		Graph_Cut_Process(G, pos_);

		delete(G);
	}
	seams = Mat::zeros(txt_height_, txt_width_, CV_8UC3);
	for (int j = 0; j < txt_width_;j++)
	for (int i = 0; i < txt_height_;i++)
	if (seams_mark_v_[i][j] > EPS || seams_mark_h_[i][j]>EPS)
	{
		seams.at<cv::Vec3b>(i, j)(0) = 255;
		seams.at<cv::Vec3b>(i, j)(1) = 255;
		seams.at<cv::Vec3b>(i, j)(2) = 255;
	}
	return output_image_;
}

void Synthesis::Graph_Generate(GraphType *G, vector<int> pos)
{
	nodes.clear();
	int i, j, k = 0, source = 0, sink = 0; // k means the kth vertex in the grapgh G
	for (i = pos[0] + pos[3]; i < pos[0] + +pos[3]+pos[4]; i++)
	for (j = pos[1] + pos[5]; j < pos[1] + pos[5]+pos[6]; j++)
	{
		if (image_mask_[j%txt_height_][i%txt_width_])	//overlap happens
		{
			nodes.push_back(G->add_node());
			image_mask_[j%txt_height_][i%txt_width_] = k + 1;	//record the position of the kth vertex of the graph
			if (Is_Belong_SOURCE(j, i))
			{
				if (k >= nodes.size())
					cout << "ERROR!" << endl;
				G->add_tweights(nodes[k], INF, 0);
				source++;
				//cout << 1 << endl;
			}
			else if (Is_Belong_SINK(pos, j, i))
			{
				if (k >= nodes.size())
					cout << "ERROR!" << endl;
				G->add_tweights(nodes[k], 0, INF);
				sink++;
				//cout << 2 << endl;
			}

			/* Remark:
			To avoid constructing the same edge several times,
			we only only add edges for top and left pixels
			*/

			// the left edge
			int nodex;
			int x = i % txt_width_;
			int y = j % txt_height_;
			float weight_;
			if (i > pos[0] + pos[3])	// in the patch
			{
				if (image_mask_[j%txt_height_][(i - 1+txt_width_) % txt_width_] > 0)		// the left is uncolored(empty) 
				{
					//cout << "1 start!" << endl;
					int tmpt_coord_ = x*txt_height_ + y;
					if (tmpt_coord_ >= seam_nodes_h_.size())
						cout << "ERROR!" << endl;
					if (seam_nodes_h_[tmpt_coord_].flag)
						//there is seam nodes(horizontal) lying on the edge (j,i-1) and (j,i)
					{
						nodes.push_back(G->add_node());// add the seam node to the graph
						k++;
						if (tmpt_coord_ >= seam_nodes_h_.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_h_[tmpt_coord_].As_Sr, seam_nodes_h_[tmpt_coord_].As_Sb, seam_nodes_h_[tmpt_coord_].As_Sg,
							seam_nodes_h_[tmpt_coord_].At_Sr, seam_nodes_h_[tmpt_coord_].At_Sb, seam_nodes_h_[tmpt_coord_].At_Sg,
							seam_nodes_h_[tmpt_coord_].As_Tr, seam_nodes_h_[tmpt_coord_].As_Tb, seam_nodes_h_[tmpt_coord_].As_Tg,
							seam_nodes_h_[tmpt_coord_].At_Tr, seam_nodes_h_[tmpt_coord_].At_Tb, seam_nodes_h_[tmpt_coord_].At_Tg
							);
						G->add_tweights(nodes[k], weight_, 0);
						seam_nodes_h_[tmpt_coord_].nodex = k;

						// The left edge
						nodex = image_mask_[j%txt_height_][ (i - 1+txt_width_) % txt_width_] - 1;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_h_[tmpt_coord_].As_Sr, seam_nodes_h_[tmpt_coord_].As_Sb, seam_nodes_h_[tmpt_coord_].As_Sg,
							input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(0), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(1), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(2),
							seam_nodes_h_[tmpt_coord_].As_Tr, seam_nodes_h_[tmpt_coord_].As_Tb, seam_nodes_h_[tmpt_coord_].As_Tg,
							input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);

						// The right edge
						nodex = image_mask_[j%txt_height_][i % txt_width_] - 1;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_h_[tmpt_coord_].At_Sr, seam_nodes_h_[tmpt_coord_].At_Sb, seam_nodes_h_[tmpt_coord_].At_Sg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3] - 1)(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3] - 1)(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3] - 1)(2),
							seam_nodes_h_[tmpt_coord_].At_Tr, seam_nodes_h_[tmpt_coord_].At_Tb, seam_nodes_h_[tmpt_coord_].At_Tg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);

						//cout << "1 end!" << endl;

					}
					else
						// just add the edge with the left pixel
					{
						nodex = image_mask_[y][ (i - 1+txt_width_) % txt_width_] - 1;
						//cout << "2 start!" << endl;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(0), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(1), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3] - 1)(2),
							output_image_.at<cv::Vec3b>(y, (i - 1 + txt_width_) % txt_width_)(0), output_image_.at<cv::Vec3b>(y, (i - 1 + txt_width_) % txt_width_)(1), output_image_.at<cv::Vec3b>(y, (i - 1 + txt_width_) % txt_width_)(2),
							input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1]-pos[5], i - pos[0]-pos[3])(2),
							output_image_.at<cv::Vec3b>(y, x)(0), output_image_.at<cv::Vec3b>(y, x)(1), output_image_.at<cv::Vec3b>(y,x)(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);
						//cout << "2 end!" << endl;
					}
				}
			}

			// The top edge
			if (j > pos[1] + pos[5])	// in the patch
			{
				if (image_mask_[(j - 1+txt_height_) % txt_height_][ i % txt_width_] > 0)		// the left is uncolored(empty) 
				{
					//cout << "3 start!" << endl;
					int tmpt_coord_ = x*txt_height_ + y;
					if (tmpt_coord_ >= seam_nodes_v_.size())
						cout << "ERROR!" << endl;
					if (seam_nodes_v_[tmpt_coord_].flag)
						//there is seam nodes(horizontal) lying on the edge (j,i-1) and (j,i)
					{
						nodes.push_back(G->add_node());// add the seam node to the graph
						k++;
						if (tmpt_coord_ >= seam_nodes_v_.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_v_[tmpt_coord_].As_Sr, seam_nodes_v_[tmpt_coord_].As_Sb, seam_nodes_v_[tmpt_coord_].As_Sg,
							seam_nodes_v_[tmpt_coord_].At_Sr, seam_nodes_v_[tmpt_coord_].At_Sb, seam_nodes_v_[tmpt_coord_].At_Sg,
							seam_nodes_v_[tmpt_coord_].As_Tr, seam_nodes_v_[tmpt_coord_].As_Tb, seam_nodes_v_[tmpt_coord_].As_Tg,
							seam_nodes_v_[tmpt_coord_].At_Tr, seam_nodes_v_[tmpt_coord_].At_Tb, seam_nodes_v_[tmpt_coord_].At_Tg
							);
						G->add_tweights(nodes[k], weight_, 0);
						seam_nodes_v_[tmpt_coord_].nodex = k;

						// The top edge
						nodex = image_mask_[(j - 1+txt_height_) % txt_height_][ i % txt_width_] - 1;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_v_[tmpt_coord_].As_Sr, seam_nodes_v_[tmpt_coord_].As_Sb, seam_nodes_v_[tmpt_coord_].As_Sg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(2),
							seam_nodes_v_[tmpt_coord_].As_Tr, seam_nodes_v_[tmpt_coord_].As_Tb, seam_nodes_v_[tmpt_coord_].As_Tg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);

						// The down edge
						nodex = image_mask_[j%txt_height_][i % txt_width_] - 1;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(seam_nodes_v_[tmpt_coord_].At_Sr, seam_nodes_v_[tmpt_coord_].At_Sb, seam_nodes_v_[tmpt_coord_].At_Sg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(2),
							seam_nodes_v_[tmpt_coord_].At_Tr, seam_nodes_v_[tmpt_coord_].At_Tb, seam_nodes_v_[tmpt_coord_].At_Tg,
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);
						//cout << "3 end!" << endl;
					}
					else
						// just add the edge with the left pixel
					{
						//cout << "4 start!" << endl;
						nodex = image_mask_[(j - 1+txt_height_) % txt_height_][x] - 1;
						if (nodex >= nodes.size() || k >= nodes.size())
							cout << "ERROR!" << endl;
						weight_ = Edge_Weight(input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5] - 1, i - pos[0] - pos[3])(2),
							output_image_.at<cv::Vec3b>((j - 1 + txt_height_) % txt_height_, x)(0), output_image_.at<cv::Vec3b>((j - 1 + txt_height_) % txt_height_, x)(1), output_image_.at<cv::Vec3b>((j - 1 + txt_height_) % txt_height_, x)(2),
							input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(0), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(1), input_texture_.at<cv::Vec3b>(j - pos[1] - pos[5], i - pos[0] - pos[3])(2),
							output_image_.at<cv::Vec3b>(y,x)(0), output_image_.at<cv::Vec3b>(y,x)(1), output_image_.at<cv::Vec3b>(y,x)(2));
						G->add_edge(nodes[k], nodes[nodex], weight_, weight_);
						//cout << "4 end!" << endl;
					}

				}
			}
			k++;

		}

	}
}

void Synthesis::Graph_Cut_Process(GraphType* G, vector<int> pos)
{
	bool is_first_ovlp_;				// This flag aims to ensure the flag "is_last_source_nd_" changes at most one time
	bool is_last_source_nd_ = true;	// Record the last node which belongs to source part after min cutting processing in each column
	int i, j, temp_index_;
	int x, y, nodex,left_nodex_,sources=0,sinks=0;
	int left_x_;
	int node_size_ = nodes.size();
	for (i = 0; i < pos[4]; i++)
	{
		is_first_ovlp_ = true;
		for (j = 0; j < pos[6]; j++)
		{
			x = (i + pos[0]+pos[3]) % txt_width_;
			y = (j + pos[1]+pos[5]) % txt_height_;
			temp_index_ = x*txt_height_ + y;
			nodex = image_mask_[y][x];
			if (nodex > node_size_)
				cout << "ERROR" << endl;
			//seams_mark_h_[y][x] = seams_mark_v_[y][x] = 0;
			if (nodex == 0) // This pixel is uncolored
			{
				output_image_.at<cv::Vec3b>(y, x)(0) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
				output_image_.at<cv::Vec3b>(y, x)(1) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
				output_image_.at<cv::Vec3b>(y, x)(2) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);
				image_mask_[y][x] = INF;
				colored_pixels_++;
				if (colored_pixels_ >= total_pixels_)
				{
					cout << "Finished!" << endl;
					return;
				}
			}
			else
			{
				if (is_first_ovlp_)
				{
					if (G->what_segment(nodes[nodex - 1]) == GraphType::SOURCE)
						is_last_source_nd_ = true;
					else
						is_last_source_nd_ = false;
					is_first_ovlp_ = false;
				}

				// When the nodes belongs to the source part
				if (G->what_segment(nodes[nodex - 1]) == GraphType::SOURCE)
				{
					// This seam should be removed
					// seams_mark_h_[y][x] = seams_mark_v_[y][x] = 0;
					sources++;
					output_image_.at<cv::Vec3b>(y, x)(0) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
					output_image_.at<cv::Vec3b>(y, x)(1) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
					output_image_.at<cv::Vec3b>(y, x)(2) = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);
					

					if (!is_last_source_nd_) // Generating a new seam node
					{
						is_last_source_nd_ = true;
						
						if (temp_index_ > seam_nodes_v_.size())
							cout << "ERROR" << endl;
						if (seam_nodes_v_[temp_index_].flag && seam_nodes_v_[temp_index_].nodex!=-1)
						{
							// The old seam won't be preserved
							if (G->what_segment(nodes[seam_nodes_v_[temp_index_].nodex]) == GraphType::SOURCE)
								seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] = 0;
						}

						// The top pixel(output image)
						seam_nodes_v_[temp_index_].As_Sr = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(0);
						seam_nodes_v_[temp_index_].As_Sb = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(1);
						seam_nodes_v_[temp_index_].As_Sg = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(2);

						// The top pixel(input texture)
						seam_nodes_v_[temp_index_].At_Sr = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(0);
						seam_nodes_v_[temp_index_].At_Sb = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(1);
						seam_nodes_v_[temp_index_].At_Sg = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(2);

						// The down pixel(output image)
						seam_nodes_v_[temp_index_].As_Tr = output_image_.at<cv::Vec3b>(y, x)(0);
						seam_nodes_v_[temp_index_].As_Tb = output_image_.at<cv::Vec3b>(y, x)(1);
						seam_nodes_v_[temp_index_].As_Tg = output_image_.at<cv::Vec3b>(y, x)(2);

						// The down pixel(input iamge)
						seam_nodes_v_[temp_index_].At_Tr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
						seam_nodes_v_[temp_index_].At_Tb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
						seam_nodes_v_[temp_index_].At_Tg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);

						seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] += Edge_Weight(seam_nodes_v_[temp_index_].As_Sr, seam_nodes_v_[temp_index_].As_Sb, seam_nodes_v_[temp_index_].As_Sg,
							seam_nodes_v_[temp_index_].At_Sr, seam_nodes_v_[temp_index_].At_Sb, seam_nodes_v_[temp_index_].At_Sg,
							seam_nodes_v_[temp_index_].As_Tr, seam_nodes_v_[temp_index_].As_Tb, seam_nodes_v_[temp_index_].As_Tg,
							seam_nodes_v_[temp_index_].At_Tr, seam_nodes_v_[temp_index_].At_Tb, seam_nodes_v_[temp_index_].At_Tg
							);
						seam_nodes_v_[temp_index_].flag = 1;
						seam_nodes_v_[temp_index_].nodex = -1;
					}
					else
					{
						seam_nodes_v_[x*txt_height_ + y].flag = 0;
						seam_nodes_v_[x*txt_height_ + y].nodex = -1;
						seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] = 0; // Remove the seam if exists
					}
					// Check the left pixel. If its origin is different from the current one(SOURCE), then add a seam node
					left_x_ = (i + pos[0] + pos[3] - 1+txt_width_) % txt_width_;
					left_nodex_ = image_mask_[y][left_x_];
					
					if (temp_index_ > seam_nodes_h_.size())
						cout << "ERROR" << endl;
					if (left_nodex_&&left_nodex_<=node_size_)	// the left node is colored and is the node of the graph
					{
						if (G->what_segment(nodes[left_nodex_ - 1]) == GraphType::SINK)
						{
							
							if (seam_nodes_h_[temp_index_].flag && seam_nodes_h_[temp_index_].nodex!=-1)
							{
								// The old seam won't be preserved
								if (G->what_segment(nodes[seam_nodes_h_[temp_index_].nodex]) == GraphType::SOURCE)
									seams_mark_h_[y][left_x_] = 0;
							}

							seam_nodes_h_[temp_index_].flag = 1;
							seam_nodes_h_[temp_index_].nodex = -1;

							// The top pixel(input image)
							seam_nodes_h_[temp_index_].As_Sr = output_image_.at<cv::Vec3b>(y, left_x_)(0);
							seam_nodes_h_[temp_index_].As_Sb = output_image_.at<cv::Vec3b>(y, left_x_)(1);
							seam_nodes_h_[temp_index_].As_Sg = output_image_.at<cv::Vec3b>(y, left_x_)(2);


							// The top pixel(output texture)
							seam_nodes_h_[temp_index_].At_Sr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(0);
							seam_nodes_h_[temp_index_].At_Sb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(1);
							seam_nodes_h_[temp_index_].At_Sg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(2);

							// The down pixel(output image)
							seam_nodes_h_[temp_index_].As_Tr = output_image_.at<cv::Vec3b>(y, x)(0);
							seam_nodes_h_[temp_index_].As_Tb = output_image_.at<cv::Vec3b>(y, x)(1);
							seam_nodes_h_[temp_index_].As_Tg = output_image_.at<cv::Vec3b>(y, x)(2);

							// The down pixel(input iamge)
							seam_nodes_h_[temp_index_].At_Tr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
							seam_nodes_h_[temp_index_].At_Tb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
							seam_nodes_h_[temp_index_].At_Tg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);

							seams_mark_h_[y][left_x_] += Edge_Weight(seam_nodes_h_[temp_index_].As_Sr, seam_nodes_h_[temp_index_].As_Sb, seam_nodes_h_[temp_index_].As_Sg,
								seam_nodes_h_[temp_index_].At_Sr, seam_nodes_h_[temp_index_].At_Sb, seam_nodes_h_[temp_index_].At_Sg,
								seam_nodes_h_[temp_index_].As_Tr, seam_nodes_h_[temp_index_].As_Tb, seam_nodes_h_[temp_index_].As_Tg,
								seam_nodes_h_[temp_index_].At_Tr, seam_nodes_h_[temp_index_].At_Tb, seam_nodes_h_[temp_index_].At_Tg
								);
							
						}
						else
						{
							seam_nodes_h_[temp_index_].flag = 0;
							seam_nodes_h_[temp_index_].nodex = -1;
							seams_mark_h_[y][left_x_] = 0;
						}
					}

				}
					

				// When the nodes belongs to the sink part
				if (G->what_segment(nodes[nodex - 1]) == GraphType::SINK)
				{
					sinks++;
					
					if (is_last_source_nd_) // Generaating a new seam node
					{
						is_last_source_nd_ = false;
						if (temp_index_ > seam_nodes_v_.size())
							cout << "ERROR" << endl;
						if (seam_nodes_v_[temp_index_].flag && seam_nodes_v_[temp_index_].nodex!=-1)
						{
							// The old seam won't be preserved
							if (G->what_segment(nodes[seam_nodes_v_[temp_index_].nodex]) == GraphType::SOURCE)
								seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] = 0;
						}

						seam_nodes_v_[temp_index_].flag = 1;
						seam_nodes_v_[temp_index_].nodex = -1;

						// The top pixel(output image)
						seam_nodes_v_[temp_index_].At_Sr = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(0);
						seam_nodes_v_[temp_index_].At_Sb = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(1);
						seam_nodes_v_[temp_index_].At_Sg = output_image_.at<cv::Vec3b>((y - 1 + txt_height_) % txt_height_, x)(2);


						// The top pixel(input texture)
						seam_nodes_v_[temp_index_].As_Sr = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(0);
						seam_nodes_v_[temp_index_].As_Sb = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(1);
						seam_nodes_v_[temp_index_].As_Sg = input_texture_.at<cv::Vec3b>((j + pos[5] - 1 + pat_height_) % pat_height_, (i + pos[3]) % pat_width_)(2);

						// The down pixel(output image)
						seam_nodes_v_[temp_index_].At_Tr = output_image_.at<cv::Vec3b>(y, x)(0);
						seam_nodes_v_[temp_index_].At_Tb = output_image_.at<cv::Vec3b>(y, x)(1);
						seam_nodes_v_[temp_index_].At_Tg = output_image_.at<cv::Vec3b>(y, x)(2);

						// The down pixel(input iamge)
						seam_nodes_v_[temp_index_].As_Tr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
						seam_nodes_v_[temp_index_].As_Tb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
						seam_nodes_v_[temp_index_].As_Tg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);
					
						seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] += Edge_Weight(seam_nodes_v_[temp_index_].As_Sr, seam_nodes_v_[temp_index_].As_Sb, seam_nodes_v_[temp_index_].As_Sg,
							seam_nodes_v_[temp_index_].At_Sr, seam_nodes_v_[temp_index_].At_Sb, seam_nodes_v_[temp_index_].At_Sg,
							seam_nodes_v_[temp_index_].As_Tr, seam_nodes_v_[temp_index_].As_Tb, seam_nodes_v_[temp_index_].As_Tg,
							seam_nodes_v_[temp_index_].At_Tr, seam_nodes_v_[temp_index_].At_Tb, seam_nodes_v_[temp_index_].At_Tg
							);
						
					}
					else
					{
						if (seam_nodes_v_[temp_index_].flag && seam_nodes_v_[temp_index_].nodex!=-1)
						{
							// The old seam won't be preserved
							if (G->what_segment(nodes[seam_nodes_v_[temp_index_].nodex]) == GraphType::SOURCE)
							{
								seams_mark_v_[(y - 1 + txt_height_) % txt_height_][x] = 0;
								seam_nodes_v_[x*txt_height_ + y].flag = 0;
							}
						}
						seam_nodes_v_[x*txt_height_ + y].nodex = -1;
					}

					// Check the left pixel. If its origin is different from the current one(SOURCE), then add a seam node
					left_x_ = (i + pos[0] + pos[3] - 1+txt_width_) % txt_width_;
					left_nodex_ = image_mask_[y][left_x_];
					
					if (temp_index_ > seam_nodes_v_.size())
						cout << "ERROR" << endl;
					if (left_nodex_ && left_nodex_<=node_size_)
					{
						if (G->what_segment(nodes[left_nodex_-1]) == GraphType::SOURCE)
						{
							
							if (seam_nodes_h_[temp_index_].flag && seam_nodes_h_[temp_index_].nodex!=-1)
							{
								// The old seam won't be preserved
								if (G->what_segment(nodes[seam_nodes_h_[temp_index_].nodex]) == GraphType::SOURCE)
									seams_mark_h_[y][left_x_] = 0;
							}
							seam_nodes_h_[temp_index_].flag = 1;
							//seam_nodes_h_[temp_index_].nodex = -1;

							// The left pixel(output image)
							seam_nodes_h_[temp_index_].At_Sr = output_image_.at<cv::Vec3b>(y, left_x_)(0);
							seam_nodes_h_[temp_index_].At_Sb = output_image_.at<cv::Vec3b>(y, left_x_)(1);
							seam_nodes_h_[temp_index_].At_Sg = output_image_.at<cv::Vec3b>(y, left_x_)(2);

							// The top pixel(input texture)
							seam_nodes_h_[temp_index_].As_Sr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(0);
							seam_nodes_h_[temp_index_].As_Sb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(1);
							seam_nodes_h_[temp_index_].As_Sg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3] - 1 + pat_width_) % pat_width_)(2);

							// The down pixel(output image)
							seam_nodes_h_[temp_index_].At_Tr = output_image_.at<cv::Vec3b>(y, x)(0);
							seam_nodes_h_[temp_index_].At_Tb = output_image_.at<cv::Vec3b>(y, x)(1);
							seam_nodes_h_[temp_index_].At_Tg = output_image_.at<cv::Vec3b>(y, x)(2);

							// The down pixel(input iamge)
							seam_nodes_h_[temp_index_].As_Tr = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(0);
							seam_nodes_h_[temp_index_].As_Tb = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(1);
							seam_nodes_h_[temp_index_].As_Tg = input_texture_.at<cv::Vec3b>((j + pos[5]) % pat_height_, (i + pos[3]) % pat_width_)(2);

							seams_mark_h_[y][left_x_] += Edge_Weight(seam_nodes_h_[temp_index_].As_Sr, seam_nodes_h_[temp_index_].As_Sb, seam_nodes_h_[temp_index_].As_Sg,
								seam_nodes_h_[temp_index_].At_Sr, seam_nodes_h_[temp_index_].At_Sb, seam_nodes_h_[temp_index_].At_Sg,
								seam_nodes_h_[temp_index_].As_Tr, seam_nodes_h_[temp_index_].As_Tb, seam_nodes_h_[temp_index_].As_Tg,
								seam_nodes_h_[temp_index_].At_Tr, seam_nodes_h_[temp_index_].At_Tb, seam_nodes_h_[temp_index_].At_Tg
								);
							
						}
						else
						{
							if (seam_nodes_h_[x*txt_height_ + y].flag && seam_nodes_h_[temp_index_].nodex!=-1)
							{
								// The old seam won't be preserved
								if (G->what_segment(nodes[seam_nodes_h_[temp_index_].nodex]) == GraphType::SOURCE)
								{
									seams_mark_h_[y][left_x_] = 0;
									seam_nodes_h_[x*txt_height_ + y].flag = 0;
								}
							}
							seam_nodes_h_[x*txt_height_ + y].nodex = -1;
						}
					}
					
				}	
			}
		}
	}
}

bool Synthesis::Is_Belong_SOURCE(int i, int j)
{
	for (int k = -1; k <= 1;k++)
	for (int l = -1; l <= 1;l++)
	if (image_mask_[(i+k+txt_height_) % txt_height_][(j+l+txt_width_) % txt_width_] == 0) 
		// the pixel has an empty neighbor(uncolored) and in the patch(assumption)
		return true;
	return false;
}

bool Synthesis::Is_Belong_SINK(vector<int> pos, int i, int j)
{
	if (j == pos[0] + pos[3])
		return true;
	unsigned int x = pos[0] + pos[4] - 1;
	if (x  > txt_width_) 
	{
		if (j == x - 1)
			return true;
	}
	else if (j == x)
		return true;

	if (i == pos[1] + pos[5])
		return true;
	unsigned int y = pos[1] + pos[6] - 1;
	if (y  > txt_height_) 
	{
		if (i == y - 1)
			return true;
	}
	else if (i == y)
		return true;

	return false;
}

float Synthesis::Edge_Weight(int Out_Sr, int Out_Sb, int Out_Sg, int in_Sr, int in_Sb, int in_Sg,
						   int Out_Er, int Out_Eb, int Out_Eg, int in_Er, int in_Eb, int in_Eg)
{
	float weight_ = sqrt(pow(Out_Sr - in_Sr, 2.0) + pow(Out_Sb - in_Sb, 2.0) + pow(Out_Sg - in_Sg, 2.0));
	weight_ = weight_ + sqrt(pow(Out_Er - in_Er, 2.0) + pow(Out_Eb - in_Eb, 2.0) + pow(Out_Eg - in_Eg, 2.0));
	return weight_;
}