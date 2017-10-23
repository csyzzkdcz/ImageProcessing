#include"Warping_IDW.h"
#include<iostream>
using namespace std;
Warping_IDW::Warping_IDW()
{

}

Warping_IDW::~Warping_IDW()
{

}



double Warping_IDW::calculate_sigma_(QPointF point_, QPointF base_point_)
{
	double dist = distance(point_, base_point_);
	dist = dist > DIST_MIN ? dist : DIST_MIN;
	double sigma,temp;
	temp = RADUIS - dist > 0 ? RADUIS-dist : 0;
	sigma = pow(temp / (RADUIS*dist),MIU);
	sigma = 1/(dist*dist);
	return sigma;
}

void Warping_IDW::DoWarping_(QImage &image_)
{
	SetFixedPoints(image_);
	//for(int i=0;i<image_.width();i++)
		//image_.setPixel(i,255,image_.pixel(255,i)); 
	QImage *new_image_;
	new_image_ = new QImage(image_);
	int width = new_image_->width();
	int height = new_image_->height();
	Is_Colored_.setZero(width,height);

	int size_  = start_points_.size();
	int i,j;
	double sigma_ij_,dist_ij_;
	vector<double> t11,t12,t21,t22;
	Vector4d T;
	// calculate Ti
	for(i=0;i<size_;i++)
	{
		Calculate_Coef_Matrix_(i,T);
		
		t11.push_back(T(0));
		t12.push_back(T(1));
		t21.push_back(T(2));
		t22.push_back(T(3));
		/*Ti =  t11(i),t12(i)
				t21(i),t22(i)*/
	}
	//transfer the current image into new image

	vector<double> sigma_;
	Matrix2d Tk;
	double weight_k_;
	double sum_;
	for(i=0;i<width;i++)
		for(j=0;j<height;j++)
			new_image_->setPixel(i,j,qRgb(255,255,255));	//initialize the new image
	for(i=0;i<width;i++)
		for(j=0;j<height;j++)
		{
			sigma_.clear();
			QPointF current_point_;
			current_point_.setX(i);
			current_point_.setY(j);
			for(int k=0;k<size_;k++)
				sigma_.push_back(calculate_sigma_(current_point_,start_points_[k]));
			sum_ = calculate_sum_(sigma_);
			Vector2d f;
			f<< 0,
				0;
			for(int k=0;k<size_;k++)
			{
				Tk << t11[k],t12[k],
					  t21[k],t22[k];
				weight_k_ = sigma_[k]/sum_;
				Vector2d p,pk,qk;
				p<< i,
					j;
				pk<< start_points_[k].rx(),
					 start_points_[k].ry();
				qk<< end_points_[k].rx(),
					 end_points_[k].ry();
				f = f + weight_k_ * (qk+Tk*(p-pk));
			}
			// Find the nearest integers for f(0)&& f(1)
			int tmpt_x_ = Find_Nearest_Int_(f(0));
			int tmpt_y_ = Find_Nearest_Int_(f(1));
			if (tmpt_x_>=0 && tmpt_x_<= width-1 && tmpt_y_>=0 && tmpt_y_<= height-1)
			{
				new_image_->setPixel(tmpt_x_,tmpt_y_,image_.pixel(i,j));
				Is_Colored_(tmpt_x_,tmpt_y_) = 1;
			}
		}
		image_ = *new_image_;
		delete(new_image_);
		Fill_White_Holes(image_);
}

void Warping_IDW::Calculate_Coef_Matrix_(int i, Vector4d &T)
{
	int size_  = start_points_.size();
	double sigma_ij_;
	Matrix4d A;
	Vector4d d;
	// the derivation equation is AT=d,by solving which for every i, we can get T
	A << 0,0,0,0,
		 0,0,0,0,
		 0,0,0,0,
		 0,0,0,0;
	d << 0,
		 0,
		 0,
		 0;
	for(int j=0;j<size_;j++)
		{
			if(j!=i)
			{
				sigma_ij_ = calculate_sigma_(start_points_[i], start_points_[j]);

				A(0,0) = A(0,0) + sigma_ij_ * (start_points_[j].rx() - start_points_[i].rx())*(start_points_[j].rx() - start_points_[i].rx());
				A(0,1) = A(0,1) + sigma_ij_ * (start_points_[j].rx() - start_points_[i].rx())*(start_points_[j].ry() - start_points_[i].ry());
				d(0) = d(0) + sigma_ij_ * (end_points_[j].rx() - end_points_[i].rx())*(start_points_[j].rx() - start_points_[i].rx());
				// the fist equation is A(0,0)t11 + A(0,1)t12 = d(0)

				A(1,0) = A(1,0) + sigma_ij_ * (start_points_[j].rx() - start_points_[i].rx())*(start_points_[j].ry() - start_points_[i].ry());
				A(1,1) = A(1,1) + sigma_ij_ * (start_points_[j].ry() - start_points_[i].ry())*(start_points_[j].ry() - start_points_[i].ry());
				d(1) = d(1) + sigma_ij_ * (end_points_[j].rx() - end_points_[i].rx())*(start_points_[j].ry() - start_points_[i].ry());
				// the second equation is A(1,0)t11 + A(1,1)t12 = d(1)

				A(2,2) = A(0,0);
				A(2,3) = A(0,1);
				d(2) = d(2) + sigma_ij_ * (end_points_[j].ry() - end_points_[i].ry())*(start_points_[j].rx() - start_points_[i].rx());
				// the third equation is A(2,2)t21 + A(2,3)t22 = d(2)

				A(3,2) = A(1,0);
				A(3,3) = A(1,1);
				d(3) = d(3) + sigma_ij_ * (end_points_[j].ry() - end_points_[i].ry())*(start_points_[j].ry() - start_points_[i].ry());
				// the last equation is A(3,2)t21 + A(3,3)t22 = d(3)
			}
		}
		T=A.colPivHouseholderQr().solve(d);
}