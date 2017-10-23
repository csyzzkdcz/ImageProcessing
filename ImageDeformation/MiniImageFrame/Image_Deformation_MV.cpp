#include"Image_Deformation_MV.h"
Image_Deformation_MV::Image_Deformation_MV()
{
}

Image_Deformation_MV::~Image_Deformation_MV()
{
}

void Image_Deformation_MV::Deformation(cv::Mat image_mat_,cv::Mat &target_image_mat_,PolygonCage *poly_cage_,PolygonCage* target_poly_cage_)
{
	double Min_area_ = target_poly_cage_->BBox_.width() * target_poly_cage_->BBox_.height() / 40.0 ;		// the min area for triangular frame
	double Max_area_ = target_poly_cage_->BBox_.width() * target_poly_cage_->BBox_.height() / 20.0 ;		// the max area for triangular frame
	//triangulation_method_->Triangulation_with_rand(target_poly_cage_,triangle_frame_);
	triangulation_method_ ->Triangulation_with_area_constraint(target_poly_cage_, Min_area_, Max_area_,triangle_frame_);
	//triangulation_method_->Triangulation_with_angle_constraint(target_poly_cage_,triangle_frame_);
	int i;
	vector<double> init_poss_x_, init_poss_y_, cur_poss_x_,cur_poss_y_;
	double sum_weight_;
	init_poss_x_.clear();
	init_poss_y_.clear();
	cur_poss_x_.clear();
	cur_poss_y_.clear();
	init_poss_x_.resize(triangle_frame_.numberofpoints);
	init_poss_y_.resize(triangle_frame_.numberofpoints);
	cur_poss_x_.resize(triangle_frame_.numberofpoints);
	cur_poss_y_.resize(triangle_frame_.numberofpoints);

	for(i=0;i<target_poly_cage_->vertices_.size();i++)				// The vertices in the polycage
	{
		init_poss_x_[i] = poly_cage_->vertices_[i].rx();
		init_poss_y_[i] = poly_cage_->vertices_[i].ry();
		cur_poss_x_[i] = target_poly_cage_->vertices_[i].rx();
		cur_poss_y_[i] = target_poly_cage_->vertices_[i].ry();
	}
	for(i=target_poly_cage_->vertices_.size();i<triangle_frame_.numberofpoints;i++)
	{
		double init_pos_x_=0,init_pos_y_=0,sum_weight_ = 0;
		for(int j=0;j<target_poly_cage_->vertices_.size();j++)
		{
			double dist_j_ = dist(triangle_frame_.pointlist[2*i],triangle_frame_.pointlist[2*i+1],target_poly_cage_->vertices_[j].rx(),target_poly_cage_->vertices_[j].ry());
			double half_tan_rj_ = calculate_half_tangent(triangle_frame_.pointlist[2*i],triangle_frame_.pointlist[2*i+1],target_poly_cage_->vertices_[j].rx(),target_poly_cage_->vertices_[j].ry(),target_poly_cage_->vertices_[(j-1+target_poly_cage_->vertices_.size())%target_poly_cage_->vertices_.size()].rx(),target_poly_cage_->vertices_[(j-1+target_poly_cage_->vertices_.size())%target_poly_cage_->vertices_.size()].ry());
			double half_tan_bj_ = calculate_half_tangent(triangle_frame_.pointlist[2*i],triangle_frame_.pointlist[2*i+1],target_poly_cage_->vertices_[j].rx(),target_poly_cage_->vertices_[j].ry(),target_poly_cage_->vertices_[(j+1)%target_poly_cage_->vertices_.size()].rx(),target_poly_cage_->vertices_[(j+1)%target_poly_cage_->vertices_.size()].ry());
			double weight_j_ = (half_tan_rj_+half_tan_bj_)/dist_j_;
			init_pos_x_ = init_pos_x_ + weight_j_*poly_cage_->vertices_[j].rx();
			init_pos_y_ = init_pos_y_ + weight_j_*poly_cage_->vertices_[j].ry();
			sum_weight_ = sum_weight_ + weight_j_;
		}
			cur_poss_x_[i] = triangle_frame_.pointlist[2*i];
			cur_poss_y_[i] = triangle_frame_.pointlist[2*i+1];
			init_poss_x_[i] = init_pos_x_/sum_weight_;
			init_poss_y_[i] = init_pos_y_/sum_weight_;
		if(init_poss_x_[i] < 0 || init_poss_x_[i] >= image_mat_.cols || init_poss_y_[i] < 0 || init_poss_y_[i] >= image_mat_.rows)
				// dealing with out of range
		{
			init_poss_x_[i] = cur_poss_x_[i];
			init_poss_y_[i] = cur_poss_y_[i];
			target_image_mat_.at<cv::Vec3b>(Near_Int(cur_poss_y_[i]),Near_Int(cur_poss_x_[i])) = target_image_mat_.at<cv::Vec3b>(Near_Int(init_poss_y_[i]),Near_Int(init_poss_x_[i]));
		}
		else
			target_image_mat_.at<cv::Vec3b>(Near_Int(cur_poss_y_[i]),Near_Int(cur_poss_x_[i])) = image_mat_.at<cv::Vec3b>(Near_Int(init_poss_y_[i]),Near_Int(init_poss_x_[i]));
	}
	for(i=0;i<triangle_frame_.numberoftriangles;i++)
	{
		int vertex_0_ = triangle_frame_.trianglelist[i * triangle_frame_.numberofcorners];
		int vertex_1_ = triangle_frame_.trianglelist[i * triangle_frame_.numberofcorners+1];
		int vertex_2_ = triangle_frame_.trianglelist[i * triangle_frame_.numberofcorners+2];

		vector<QPoint> Triangle_Vertices_;
		Triangle_Vertices_.resize(3);
		Triangle_Vertices_[0].setX(triangle_frame_.pointlist[2*vertex_0_]);
		Triangle_Vertices_[0].setY(triangle_frame_.pointlist[2*vertex_0_+1]);

		Triangle_Vertices_[1].setX(triangle_frame_.pointlist[2*vertex_1_]);
		Triangle_Vertices_[1].setY(triangle_frame_.pointlist[2*vertex_1_+1]);

		Triangle_Vertices_[2].setX(triangle_frame_.pointlist[2*vertex_2_]);
		Triangle_Vertices_[2].setY(triangle_frame_.pointlist[2*vertex_2_+1]);

		PolygonCage* Triangle_ = new PolygonCage(Triangle_Vertices_);
		double p,q;
		double delta_;
		if(1.0 / Triangle_->BBox_.width()< 1.0 / Triangle_->BBox_.height())
			delta_ = 1.0 / Triangle_->BBox_.width();
		else
			delta_ = 1.0 / Triangle_->BBox_.height();
		for(p=0;p<=1;p = p + delta_)
			for(q=0;q<=1-p;q = q+ delta_)
			{
				double init_pos_x_ = p * init_poss_x_[vertex_0_] + q * init_poss_x_[vertex_1_] + (1-p-q) * init_poss_x_[vertex_2_];
				double init_pos_y_ = p * init_poss_y_[vertex_0_] + q * init_poss_y_[vertex_1_] + (1-p-q) * init_poss_y_[vertex_2_];
				
				double cur_pos_x_ = p * cur_poss_x_[vertex_0_] + q * cur_poss_x_[vertex_1_] + (1-p-q) * cur_poss_x_[vertex_2_];
				double cur_pos_y_ = p * cur_poss_y_[vertex_0_] + q * cur_poss_y_[vertex_1_] + (1-p-q) * cur_poss_y_[vertex_2_];

				target_image_mat_.at<cv::Vec3b>(Near_Int(cur_pos_y_),Near_Int(cur_pos_x_)) = image_mat_.at<cv::Vec3b>(Near_Int(init_pos_y_),Near_Int(init_pos_x_));

			}
	}
}