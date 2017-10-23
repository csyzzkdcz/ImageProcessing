#include"Warping_RBF.h"

Warping_RBF::Warping_RBF()
{
	//M << 0,0,
		// 0,0;
	//b << 0,
		// 0;

}

Warping_RBF::~Warping_RBF()
{

}

void Warping_RBF::DoWarping_(QImage &image_)
{
	SetFixedPoints(image_);
	Calculate_Radial_Basis_Fun_();

	int width_ = image_.width(), height_ = image_.height(),size_ = start_points_.size();
	int i,j;
	Is_Colored_.setZero(width_,height_);
	QImage* new_image_;
	new_image_ = new QImage(image_);
	for(i=0;i<width_;i++)
		for(j=0;j<height_;j++)
			new_image_->setPixel(i,j,qRgb(255,255,255));	//initialize the new image
	double new_position_x,new_position_y;
	for(i=0;i<width_;i++)
		for(j=0;j<height_;j++)
		{
			QPointF current_point_(i,j);
			Vector2d point_;
			point_ << i,
					  j;
			double sum_x = 0, sum_y = 0;
			for(int k=0;k<size_;k++)
			{
				sum_x = sum_x + alpha_x_(k)*pow(distance(current_point_,start_points_[k]) * distance(current_point_,start_points_[k]) + r*r,GAMMA*1.0/2);
				sum_y = sum_y + alpha_y_(k)*pow(distance(current_point_,start_points_[k]) * distance(current_point_,start_points_[k]) + r*r,GAMMA*1.0/2);
			}
			//new_position_x =(sum_x + (M*point_+b)[0])-(int (sum_x + (M*point_+b)[0]))>0.5? (int (sum_x + (M*point_+b)[0]))+1:(int (sum_x + (M*point_+b)[0]));
			//new_position_y =(sum_y + (M*point_+b)[1])-(int (sum_y + (M*point_+b)[1]))>0.5? (int (sum_y + (M*point_+b)[1]))+1:(int (sum_y + (M*point_+b)[1]));
			new_position_x = Find_Nearest_Int_(sum_x + (M*point_+b)(0));
			new_position_y = Find_Nearest_Int_(sum_y + (M*point_+b)(1));
			if(new_position_x >=0 && new_position_x <=width_-1 && new_position_y >=0 && new_position_y <=height_-1 )
			{
				new_image_->setPixel(new_position_x,new_position_y,image_.pixel(i,j));
				Is_Colored_(new_position_x,new_position_y) = 1;
			}
		}
	image_ = * new_image_;
	delete(new_image_);
	Fill_White_Holes(image_);
}


void Warping_RBF::Calculate_Affine_Component_()
// Compute the linear transfermation A(p) = M*p + b  that we need
/* M,b should be the best square-approximated solution to the linear equations:
         M * pi + b = qi            where i take values on 0,1...n-1
*/

{
	int size_ = start_points_.size();
	int i;
	MatrixXd Coef_Matrix(size_,3);
	VectorXd q(size_);
	Vector3d temp_;
	for(i=0;i<size_;i++)
	{
		Coef_Matrix(i,0) = start_points_[i].rx();
		Coef_Matrix(i,1) = start_points_[i].ry();
		Coef_Matrix(i,2) = 1;
		q(i) = end_points_[i].rx();
	}
	temp_ = Coef_Matrix.colPivHouseholderQr().solve(q);
	//temp_= Coef_Matrix.jacobiSvd(ComputeThinU | ComputeThinV).solve(q);
	M(0,0) = temp_(0);
	M(0,1) = temp_(1);
	b(0) = temp_(2);

	for(i=0;i<size_;i++)
		q(i) = end_points_[i].ry();
	temp_ = Coef_Matrix.colPivHouseholderQr().solve(q);
	//temp_= Coef_Matrix.jacobiSvd(ComputeThinU | ComputeThinV).solve(q);
	M(1,0) = temp_(0);
	M(1,1) = temp_(1);
	b(1) = temp_(2);
}

void Warping_RBF::Calculate_Radial_Basis_Fun_()
{
	Calculate_Affine_Component_();
	int size_ = start_points_.size();
	int i,j;
	double dist_,min_dist_ = MAX_DIST;
	dist_min_r_.resize(size_);
	// Compute ri = min{ d(pi,pj) : j!=i } for every i
	for(i=0;i<size_;i++)
	{
		for(j=0;j<size_;j++)
			if(j!=i)
			{
				dist_ = distance(start_points_[i],start_points_[j]);
				if(min_dist_ > dist_)
					min_dist_ = dist_;
			}
		dist_min_r_[i] = min_dist_;
	}
	r = 0;
	for(i=0;i<size_;i++)
		r = r + dist_min_r_[i];
	r  = r / size_;
	// Compute the coefficient for radial basis function
	alpha_x_.setZero(size_);
	alpha_y_.setZero(size_);
	MatrixXd Coef_Matrix_(size_,size_);
	VectorXd result_right_(size_);
	Vector2d temp_;
	for(i=0;i<size_;i++)
	{
		for(j=0;j<size_;j++)
		{
			dist_ = distance(start_points_[i],start_points_[j]);
			//Coef_Matrix_(i,j) =pow( pow(dist_,2.0) + pow(dist_min_r_[j],2.0),0.5);
			Coef_Matrix_(i,j) =pow( pow(dist_,2.0) + pow(r,2.0),GAMMA*1.0/2);

		}
		temp_ << start_points_[i].rx(),
				 start_points_[i].ry();
		temp_ = M * temp_ + b;
		result_right_(i) = end_points_[i].rx() - temp_(0);
	}
	alpha_x_ = Coef_Matrix_.colPivHouseholderQr().solve(result_right_);
	for(i=0;i<size_;i++)
	{
		temp_ << start_points_[i].rx(),
				 start_points_[i].ry();
		temp_ = M * temp_ + b;
		result_right_(i) = end_points_[i].ry() - temp_(1);
	}
	alpha_y_ = Coef_Matrix_.colPivHouseholderQr().solve(result_right_);
}

