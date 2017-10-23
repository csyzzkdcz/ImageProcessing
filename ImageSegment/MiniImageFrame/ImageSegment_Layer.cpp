#include"ImageSegment_Layer.h"
#ifndef K
#define K 600
#endif


bool Compare(const EDGE &edge_1, const EDGE &edge_2)	// Compare two edges
{
	return edge_1.weight_ < edge_2.weight_;
}

ImageSegment_Layer::ImageSegment_Layer()
{
	kdTree = NULL;
}

ImageSegment_Layer::ImageSegment_Layer(Mat image_mat_)
{
	kdTree = NULL;
	Init(image_mat_);
	//class_components_.clear;
	id_mat_.resize(image_mat_.rows,image_mat_.cols);
	id_sum_.resize(image_mat_.rows*image_mat_.cols);
	id_int_.resize(image_mat_.rows*image_mat_.cols);
	class_components_.resize(image_mat_.rows*image_mat_.cols);
	for (int i = 0; i < image_mat_.rows;i++)
	for (int j = 0; j < image_mat_.cols; j++)
	{
		id_mat_(i, j) = i*image_mat_.cols + j;
		id_sum_(i*image_mat_.cols + j) = 1;
		id_int_(i*image_mat_.cols + j) = K*1.0;
		class_components_[i*image_mat_.cols + j].push_back(i*image_mat_.cols + j);
	}
}

ImageSegment_Layer::~ImageSegment_Layer()
{
}


void ImageSegment_Layer::Find_Adjacent_Points_ANN_(vector<double> feature_pos_, int num_, int width_, int height_, int pos_x_, int pos_y_)
{
	if (!kdTree)
	{
		cout << "KDTree is no initialized" << endl;
		return;
	}

	ANNpoint			queryPt;				// query point
	ANNidxArray			nnIdx;					// near neighbor indices
	ANNdistArray		dists;					// near neighbor distances
	nnIdx = new ANNidx[num_];						// allocate near neigh indices
	dists = new ANNdist[num_];						// allocate near neighbor dists

	queryPt = annAllocPt(5);
	double eps = 0;
	queryPt[0] = feature_pos_[0];
	queryPt[1] = feature_pos_[1];
	queryPt[2] = feature_pos_[2];
	queryPt[3] = feature_pos_[3];
	queryPt[4] = feature_pos_[4];

	kdTree->annkSearch(						// search
		queryPt,								// query point
		num_,								// number of near neighbors
		nnIdx,								// nearest neighbors (returned)
		dists,								// distance (returned)
		eps);								// error bound
	QPointF temp_point_;
	for (int i = 1; i<num_; i++)				// Never including itself for nnIdex[0] is the index of current position
	{
		int index_x_;
		int index_y_;
		index_x_ = int(nnIdx[i] / width_);
		index_y_ = nnIdx[i] - index_x_ * width_;
		temp_point_.setX(index_x_*1.0);
		temp_point_.setY(index_y_*1.0);
		Adjacent_Points_.push_back(temp_point_);
		Adjacent_dist_.push_back(sqrt(dists[i]));
		//if (!edges_mat_(nnIdx[i], pos_x_ * width_ + pos_y_) && !edges_mat_(pos_x_ * width_ + pos_y_,nnIdx[i]))
		//{
		EDGE tmpt_edge_;
		tmpt_edge_.index_ix_ = pos_x_;
		tmpt_edge_.index_iy_ = pos_y_;
		tmpt_edge_.index_jx_ = index_x_;
		tmpt_edge_.index_jy_ = index_y_;
		tmpt_edge_.weight_ = sqrt(dists[i]);
		edges_.push_back(tmpt_edge_);
		//	edges_mat_(nnIdx[i], pos_x_ * width_ + pos_y_) = 1;
		//	edges_mat_(pos_x_ * width_ + pos_y_, nnIdx[i]) = 1;
		//}
	}
}

void ImageSegment_Layer::Generate_kdTree(Mat image_mat_)
{
	ANNpointArray		dataPts;				// data points
	int dim = 5;
	int nPts = image_mat_.rows * image_mat_.cols;
	dataPts = annAllocPts(nPts, dim);			// allocate data points
	int i, j;
	int width_ = image_mat_.cols, height_ = image_mat_.rows;
	for (i = 0; i<height_; i++)
	for (j = 0; j<width_; j++)
	{
		dataPts[i*width_ + j][0] = i*256.0 / height_;
		dataPts[i*width_ + j][1] = j*256.0 / width_;
		dataPts[i*width_ + j][2] = image_mat_.at<Vec3b>(i, j)[0];
		dataPts[i*width_ + j][3] = image_mat_.at<Vec3b>(i, j)[1];
		dataPts[i*width_ + j][4] = image_mat_.at<Vec3b>(i, j)[2];

	}
	kdTree = new ANNkd_tree(					// build search structure
		dataPts,					// the data points
		nPts,						// number of points
		dim);						// dimension of space
}


void ImageSegment_Layer::Init(Mat &image_mat_)
{
	Adjacent_Points_.clear();
	for (int i = 0; i < class_components_.size(); i++)
	{
		class_components_[i].clear();
	}
	edges_.clear();
	Gaussian_Filter(image_mat_);
	Generate_kdTree(image_mat_);
	int width_ = image_mat_.cols;
	int height_ = image_mat_.rows;
	//edges_mat_.resize(height_*width_,height_*width_);
	//edges_mat_.setZero();
	vector<double> feature_pos_;
	for (int i = 0; i < height_; i++)
	for (int j = 0; j < width_; j++)
	{
		feature_pos_.clear();
		feature_pos_.push_back(i*256.0 / height_);
		feature_pos_.push_back(j*256.0 / width_);
		feature_pos_.push_back(image_mat_.at<Vec3b>(i, j)[0]);
		feature_pos_.push_back(image_mat_.at<Vec3b>(i, j)[1]);
		feature_pos_.push_back(image_mat_.at<Vec3b>(i, j)[2]);
		Find_Adjacent_Points_ANN_(feature_pos_, ADJACENT_NUM_, width_, height_, i, j);
	}
	sort(edges_.begin(), edges_.end(), Compare);
}


void ImageSegment_Layer::Update_ClassComponents(int id_ix_, int id_iy_, int id_jx_, int id_jy_, int edge_id_)
{
	int class_i_ = id_mat_(id_ix_, id_iy_);
	int class_j_ = id_mat_(id_jx_, id_jy_);
	id_sum_(class_i_) += id_sum_(class_j_);
	// Update the size of the class
	id_int_(class_i_) = edges_[edge_id_].weight_ + K*1.0 / id_sum_(class_i_);
	//cout << id_int_(class_i_);
	id_sum_(class_j_) = 0;
	// Update the Inter-function of the class
	id_mat_(id_jx_, id_jy_) = id_mat_(id_ix_, id_iy_);		// merge the class
	int size_ = class_components_[class_j_].size();
	for (int i = 0; i < size_; i++)		// copy class j to class i
	{
		int id_ = class_components_[class_j_].at(i);
		class_components_[class_i_].push_back(id_);
		int row_ = id_ / id_mat_.cols();
		int col_ = id_ - id_mat_.cols() * row_;
		id_mat_(row_, col_) = class_i_;
	}
	class_components_[class_j_].clear();

}

void ImageSegment_Layer::DoSegment(Mat &ptr_image_)
{
	if (ptr_image_.rows > ptr_image_.cols)
	{
		cout << "#rows are more then #cols. Please transpose the image" << endl;
		return;
	}
	if (edges_.size() == 0)
		Init(ptr_image_);
	int start_q_, size_edges_ = edges_.size();
	for (start_q_ = 0; start_q_ < size_edges_; start_q_++)
	{
		int id_ix_ = edges_[start_q_].index_ix_;
		int id_iy_ = edges_[start_q_].index_iy_;
		int id_jx_ = edges_[start_q_].index_jx_;
		int id_jy_ = edges_[start_q_].index_jy_;
		if (id_mat_(id_ix_, id_iy_) != id_mat_(id_jx_, id_jy_))
		{
			if (edges_[start_q_].weight_ <= Min(id_int_(id_mat_(id_ix_, id_iy_)), id_int_(id_mat_(id_jx_, id_jy_))))
				Update_ClassComponents(id_ix_, id_iy_, id_jx_, id_jy_,start_q_);
		}
	}
	Output_Image(ptr_image_);
}

double ImageSegment_Layer::Min(double a, double b)
{
	return a < b ? a : b;
}

