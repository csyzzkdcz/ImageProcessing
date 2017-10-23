#include"Warping.h"
#include<iostream>
using namespace std;
Warping::Warping()
{

}

double Warping::distance(QPointF start_point_, QPointF end_point_)
{
	return sqrt(pow(end_point_.rx()-start_point_.rx(),2.0) + pow(end_point_.ry() - start_point_.ry(),2.0));
}

double Warping::calculate_sum_(vector<double> Array)
{
	int size_=Array.size();
	double sum_ = 0;
	for(int i=0;i<size_;i++)
		sum_ = sum_ + Array[i];
	return sum_;
}

int Warping::Find_Nearest_Int_(double x)
{
	return x-int(x) > 0.5 ? int(x) + 1 : int(x);
}

void Warping::InitControlPoints(vector<QPoint> points_start_, vector<QPoint> points_end_)
{
	int size_ = points_start_.size();
	QPointF temp_point_;
	start_points_.clear();
	end_points_.clear();
	for(int i = 0;i<size_;i++)
	{
		temp_point_.setX(double(points_start_[i].rx()));
		temp_point_.setY(double(points_start_[i].ry()));
		start_points_.push_back(temp_point_);

		temp_point_.setX(double(points_end_[i].rx()));
		temp_point_.setY(double(points_end_[i].ry()));
		end_points_.push_back(temp_point_);

	}
}

void Warping::SetFixedPoints(QImage &image_)
{
	int width_ = image_.width();
	int height_ = image_.height();
	QPointF Fixed_Point_;
	//set fixed poinst
	Fixed_Point_.setX(0);
	Fixed_Point_.setY(0);
	start_points_.push_back(Fixed_Point_);
	end_points_.push_back(Fixed_Point_);

	Fixed_Point_.setX(0);
	Fixed_Point_.setY(height_-1);
	start_points_.push_back(Fixed_Point_);
	end_points_.push_back(Fixed_Point_);

	Fixed_Point_.setX(width_-1);
	Fixed_Point_.setY(0);
	start_points_.push_back(Fixed_Point_);
	end_points_.push_back(Fixed_Point_);

	Fixed_Point_.setX(width_-1);
	Fixed_Point_.setY(height_-1);
	start_points_.push_back(Fixed_Point_);
	end_points_.push_back(Fixed_Point_);
}

void Warping::Find_Adjacent_Points_NeighBor_(int pos_x_,int pos_y_,int width_, int height_)
{
	Adjacent_Points_.clear();
	int diam = NEIGHBOR_DIST_;
	for(int k=-diam;k< diam;k++)
	for(int l=-diam;l<diam ;l++)
	{
		if(pos_x_+k<0 || pos_x_+k >= width_|| pos_y_+l<0 || pos_y_+l >= height_ || l==0 && k==0)
			continue;
		QPointF Adjacent_Point_;
		if(Is_Colored_(pos_x_+k,pos_y_+l))
		{
			Adjacent_Point_.setX((pos_x_+k)*1.0);
			Adjacent_Point_.setY((pos_y_+l)*1.0);
			Adjacent_Points_.push_back(Adjacent_Point_);
		}
	}
}

void Warping::Find_Adjacent_Points_ANN_(int pos_x_,int pos_y_,int num,int width_,int height_)
{
	Adjacent_Points_.clear();
	ANNpoint			queryPt;				// query point
	ANNidxArray			nnIdx;					// near neighbor indices
	ANNdistArray		dists;					// near neighbor distances
	nnIdx = new ANNidx[num];						// allocate near neigh indices
	dists = new ANNdist[num];						// allocate near neighbor dists

	queryPt = annAllocPt(3);
	double eps = 0;
	queryPt[0] = pos_x_;
	queryPt[1] = pos_y_;
	queryPt[2] = 0;
	kdTree->annkSearch(						// search
				queryPt,						// query point
				num,								// number of near neighbors
				nnIdx,							// nearest neighbors (returned)
				dists,							// distance (returned)
				eps);							// error bound
	QPointF temp_point_;
	for(int i=0;i<num;i++)						// Never including itself for nnIdex[0] is the index of current position
	{
		int index_x_;
		int index_y_;
		index_x_ = int(nnIdx[i]/ height_);
		index_y_ = nnIdx[i] - index_x_ * height_;
		temp_point_.setX(index_x_*1.0);
		temp_point_.setY(index_y_*1.0);
		Adjacent_Points_.push_back(temp_point_);

	}

}

void Warping::Build_kdTree(QImage image_)
{
	ANNpointArray		dataPts;				// data points
	int dim = 3;
	int nPts = image_.width() * image_.height();
	dataPts = annAllocPts(nPts, dim);			// allocate data points
	int i,j;
	int width_ = image_.width(), height_ = image_.height();
	for(i=0;i<width_;i++)
		for(j=0;j<height_;j++)
		{
			dataPts[i* height_ + j][0] = i;
			dataPts[i* height_ + j][1] = j;
			if(Is_Colored_(i,j))
				dataPts[i* height_ +j][2] = 0;
			else
				dataPts[i* height_ +j][2] = INF;	// ensure white holes can not be chosen
		}
	kdTree = new ANNkd_tree(					// build search structure
					dataPts,					// the data points
					nPts,						// number of points
					dim);						// dimension of space
}

void Warping::Fill_White_Holes(QImage &image_)
{
	int width_ = image_.width();
	int height_ = image_.height();
	int i,j;
	QImage *tmpt_image_= new QImage(image_);
	for(i=0;i<width_;i++)
		for(j =0;j<height_;j++)
		{
			tmpt_image_->setPixel(i,j,image_.pixel(i,j));
		}
	Build_kdTree(image_);	
	for(i=0;i<image_.width();i++)
		for(j =0 ;j<image_.height();j++)
		{
			
			if (Is_Colored_(i, j))
				continue;
			// Find adjacent points
				//Find_Adjacent_Points_NeighBor_(i,j,width_,height_);	
				Find_Adjacent_Points_ANN_(i,j,NEI_NUM,width_,height_);
			// using IDW to interpolate the color;
				Red_.clear();
				Blue_.clear();
				Green_.clear();
				double red_,blue_,green_;
				for(int k=0;k<Adjacent_Points_.size();k++)
				{
					QRgb color = image_.pixel(int(Adjacent_Points_[k].rx()), int(Adjacent_Points_[k].ry()));
					//cout<<Is_Colored_(int(Adjacent_Points_[k].rx()), int(Adjacent_Points_[k].ry()))<<endl;
					Red_.push_back(qRed(color)*1.0);
					Blue_.push_back(qBlue(color)*1.0);
					Green_.push_back(qGreen(color)*1.0);
				}
				//Interpolating_Ave(i,j,red_,green_,blue_);
				//Interpolating_IDW(i,j,red_,green_,blue_);
				//Interpolating_RBF(i,j,red_,green_,blue_);
				Interpolating_IDW_Ave(i,j,red_,green_,blue_);

				tmpt_image_->setPixel(i,j,qRgb(Find_Nearest_Int_(red_),Find_Nearest_Int_(green_),Find_Nearest_Int_(blue_)));
		}
		image_ = *tmpt_image_;
		delete(tmpt_image_);
}

void Warping::Interpolating_IDW(int pos_x_, int pos_y_, double &red_, double &green_, double &blue_)
{
	int i,j;
	int size_ = Adjacent_Points_.size();
	if(size_ ==0)
	{
		red_ = green_ = blue_ = 255;
		return;
	}
	double sum_sigma_ = 0;
	red_ = green_= blue_ = 0;		// Initialize the pixel black
	vector<double> sigma_;
	VectorXd  right_equ_;
	MatrixXd  A;
	MatrixXd  Ti;
	VectorXd  T;
	Vector3d  Color_new_;
	right_equ_.setZero(6);
	A.setZero(6,6);
	T.setZero(6);
	Ti.setZero(3,2);
	vector<double> t11,t12,t21,t22,t31,t32;
	double sigma_ij_;
	for(i=0;i<size_;i++)
	{
		for(j=0;j<size_;j++)
		{
			if(j==i)
				continue;
			sigma_ij_ = 1/pow(distance(Adjacent_Points_[i],Adjacent_Points_[j]),MIU);
			
			//Construct linear system
			A(0,0) = A(0,0) + sigma_ij_ * (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx());
			A(0,1) = A(0,1) + sigma_ij_ * (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry());
			right_equ_(0) = right_equ_(0) + (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Red_[j]-Red_[i]);

			A(1,0) = A(1,0) + sigma_ij_ * (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry());
			A(1,1) = A(1,1) + sigma_ij_ * (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry()) * (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry());
			right_equ_(1) = right_equ_(1) + (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry()) * (Red_[j]-Red_[i]);

			A(2,2) = A(0,0);
			A(2,3) = A(0,1);
			right_equ_(2) = right_equ_(2) + (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Green_[j]-Green_[i]);

			A(3,2) = A(1,0);
			A(3,3) = A(1,1);
			right_equ_(3) = right_equ_(3) + (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry()) * (Green_[j]-Green_[i]);

			A(4,4) = A(0,0);
			A(4,5) = A(0,1);
			right_equ_(4) = right_equ_(4) + (Adjacent_Points_[j].rx()-Adjacent_Points_[i].rx()) * (Blue_[j]-Blue_[i]);

			A(5,4) = A(1,0);
			A(5,5) = A(1,1);
			right_equ_(5) = right_equ_(5) + (Adjacent_Points_[j].ry()-Adjacent_Points_[i].ry()) * (Blue_[j]-Blue_[i]);

		}
		T = A.colPivHouseholderQr().solve(right_equ_);
		t11.push_back(T(0));
		t12.push_back(T(1));
		t21.push_back(T(2));
		t22.push_back(T(3));
		t31.push_back(T(4));
		t32.push_back(T(5));
		A.setZero(6,6);
		right_equ_.setZero(6);

	}
	for(i=0;i<size_;i++)
		sigma_.push_back(1/( pow(Adjacent_Points_[i].rx()-pos_x_,2.0) + pow(Adjacent_Points_[i].ry()-pos_y_,2.0) ));
	for(i=0;i<size_;i++)
		sum_sigma_  = sum_sigma_ + sigma_[i];
	for(i=0;i<size_;i++)
	{
		Ti << t11[i],t12[i],
			  t21[i],t22[i],
			  t31[i],t32[i];
		Vector2d p,pi;
		p<<pos_x_,
		   pos_y_;
		pi<<Adjacent_Points_[i].rx(),
			Adjacent_Points_[i].ry();

		red_ = red_ + sigma_[i] / sum_sigma_ * (Red_[i]+ (Ti * (p-pi))(0));
		green_ = green_ + sigma_[i] / sum_sigma_ * (Green_[i]+(Ti * (p-pi))(1));
		blue_ = blue_ + sigma_[i] / sum_sigma_ * (Blue_[i]+ (Ti * (p-pi))(2));
	}
}

void Warping::Interpolating_Ave(int pos_x_, int pos_y_, double &red_, double &green_, double &blue_)
{
	int i,j;
	int size_ = Adjacent_Points_.size();
	if(size_ ==0)
	{
		red_ = green_ = blue_ = 255;
		return;
	}
	red_ = green_= blue_ = 0;		// Initialize the pixel black

	for(i=0;i<size_;i++)
	{
		red_ = red_ + Red_[i];
		green_ = green_ + Green_[i];
		blue_ = blue_ + Blue_[i];
	}
	red_ = red_ / size_;
	green_ = green_/size_;
	blue_ = blue_/size_;
}

void Warping::Interpolating_IDW_Ave(int pos_x_, int pos_y_, double &red_, double &green_, double &blue_)
{
	int i,j;
	int size_ = Adjacent_Points_.size();
	if(size_ ==0)
	{
		red_ = green_ = blue_ = 255;
		return;
	}
	double sum_sigma_ = 0;
	vector<double> sigma_;

	red_ = green_= blue_ = 0;		// Initialize the pixel black

	for(i=0;i<size_;i++)
		sigma_.push_back(1/( pow(Adjacent_Points_[i].rx()-pos_x_,2.0) + pow(Adjacent_Points_[i].ry()-pos_y_,2.0) ));
	for(i=0;i<size_;i++)
		sum_sigma_  = sum_sigma_ + sigma_[i];
	for(i=0;i<size_;i++)
	{
		red_ = red_ + sigma_[i] / sum_sigma_ * Red_[i];
		green_ = green_ + sigma_[i] / sum_sigma_ * Green_[i];
		blue_ = blue_ + sigma_[i] / sum_sigma_ * Blue_[i];
	}
}

void Warping::Interpolating_RBF(int pos_x_, int pos_y_, double &red_, double &green_, double &blue_)
{
	int i,j;
	int size_ = Adjacent_Points_.size();
	if(size_ ==0)
	{
		red_ = green_ = blue_ = 255;
		return;
	}
	MatrixXd M(3,2);
	VectorXd b(3);
	// the affine matrix is Ax = Mx + b
	MatrixXd Coef_Matrix(size_,3);
	VectorXd q(size_);
	VectorXd temp_(3);
	for(i=0;i<size_;i++)
	{
		Coef_Matrix(i,0) = Adjacent_Points_[i].rx();
		Coef_Matrix(i,1) = Adjacent_Points_[i].ry();
		Coef_Matrix(i,2) = 1;
		q(i) = Red_[i];
	}
	temp_ = Coef_Matrix.colPivHouseholderQr().solve(q);
	M(0,0) = temp_(0);
	M(0,1) = temp_(1);
	b(0) = temp_(2);

	for(i=0;i<size_;i++)
		q(i) = Green_[i];
	temp_ = Coef_Matrix.colPivHouseholderQr().solve(q);
	M(1,0) = temp_(0);
	M(1,1) = temp_(1);
	b(1) = temp_(2);

	for(i=0;i<size_;i++)
		q(i) = Blue_[i];
	temp_ = Coef_Matrix.colPivHouseholderQr().solve(q);
	M(2,0) = temp_(0);
	M(2,1) = temp_(1);
	b(2) = temp_(2);

	//Calculate the radial basic function
	VectorXd alpha_x_, alpha_y_, alpha_z_;
	VectorXd dist_min_r_(size_);
	MatrixXd Coef_Matrix_(size_,size_);
	VectorXd result_right_(size_);
	Vector2d tmp_;
	double dist_,min_dist_ = MAX_DIST;

	// Compute ri = min{ d(pi,pj) : j!=i } for every i
	for(i=0;i<size_;i++)
	{
		for(j=0;j<size_;j++)
			if(j!=i)
			{
				dist_ = distance(Adjacent_Points_[i],Adjacent_Points_[j]);
				if(min_dist_ > dist_)
					min_dist_ = dist_;
			}
		dist_min_r_[i] = min_dist_;
	}
	double r = 0;
	for(i=0;i<size_;i++)
		r = r + dist_min_r_[i];
	r  = r / size_;
	// Compute the coefficient for radial basis function
	alpha_x_.setZero(size_);
	alpha_y_.setZero(size_);
	alpha_z_.setZero(size_);
	for(i=0;i<size_;i++)
	{
		for(j=0;j<size_;j++)
		{
			dist_ = distance(Adjacent_Points_[i],Adjacent_Points_[j]);
			Coef_Matrix_(i,j) =pow( pow(dist_,2.0) + pow(r,2.0),GAMMA * 1.0 /2);
		}
		tmp_ << Adjacent_Points_[i].rx(),
				 Adjacent_Points_[i].ry();
		temp_ = M * tmp_;
		temp_ = temp_ + b;
		result_right_(i) = Red_[i] - temp_(0);
	}
	alpha_x_ = Coef_Matrix_.colPivHouseholderQr().solve(result_right_);

	for(i=0;i<size_;i++)
	{
		tmp_ << Adjacent_Points_[i].rx(),
				 Adjacent_Points_[i].ry();
		temp_ = M * tmp_ + b;
		result_right_(i) = Green_[i] - temp_(1);
	}
	alpha_y_ = Coef_Matrix_.colPivHouseholderQr().solve(result_right_);

	for(i=0;i<size_;i++)
	{
		tmp_ << Adjacent_Points_[i].rx(),
				 Adjacent_Points_[i].ry();
		temp_ = M * tmp_ + b;
		result_right_(i) = Blue_[i] - temp_(2);
	}
	alpha_z_ = Coef_Matrix_.colPivHouseholderQr().solve(result_right_);

	// Calculate the interpolating color
	QPointF current_point_(i,j);
	Vector2d point_;
	point_ << i,
			  j;
	double sum_x = 0, sum_y = 0,sum_z = 0;
	for(int k=0;k<size_;k++)
	{
		sum_x = sum_x + alpha_x_(k)*pow(distance(current_point_,Adjacent_Points_[k]) * distance(current_point_,Adjacent_Points_[k]) + r*r,GAMMA*1.0/2);
		sum_y = sum_y + alpha_y_(k)*pow(distance(current_point_,Adjacent_Points_[k]) * distance(current_point_,Adjacent_Points_[k]) + r*r,GAMMA*1.0/2);
		sum_z = sum_z + alpha_z_(k)*pow(distance(current_point_,Adjacent_Points_[k]) * distance(current_point_,Adjacent_Points_[k]) + r*r,GAMMA*1.0/2);

	}
	red_ =int (sum_x + (M*point_+b)[0]);
	green_ =int (sum_y + (M*point_+b)[1]);
	blue_ = int (sum_z + (M*point_+b)[2]);
}