#include"Poisson.h"

Poisson::Poisson()
{
	is_generate_matrix_ = false;
	is_generate_vector_ = false;
}

Poisson::~Poisson()
{
	is_generate_matrix_ = false;
	is_generate_vector_ = false;
}


void Poisson::Generate_Coef_Matrix_(QRect insert_area_,MatrixXd Is_Boundary)
{
	int i,j,rel_pos_x,rel_pos_y;
	int width = insert_area_.width();
	int height = insert_area_.height();
	int points_num_ = width * height;
	int index_i,index_j;
	SparseMatrix<double> Coef_Matrix_(points_num_,points_num_);
	vector<Eigen::Triplet<int> > triplets_;
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			rel_pos_x = i + insert_area_.left();
			rel_pos_y = j + insert_area_.bottom();
			index_i = j * width + i ;
			if(Is_Boundary(j,i)>eps)
			{
				index_j = j * width + i;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, -4));

				index_j = j * width + i-1;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, 1));

				index_j = j * width + i+1;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, 1));

				index_j = (j-1) * width + i;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, 1));

				index_j = (j+1) * width + i;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, 1));
			}
			else
			{
				index_j = j * width + i;
				triplets_.push_back(Eigen::Triplet<int>(index_i, index_j, 1));
			}
		}
		Coef_Matrix_.setFromTriplets(triplets_.begin(),triplets_.end());
		Coef_Matrix_.makeCompressed();
		LU.compute(Coef_Matrix_);
		//cout<<Coef_Matrix_<<endl;
		//LLT.compute(Coef_Matrix_);
		//QR.compute(Coef_Matrix_);
		is_generate_matrix_ = true; 
}

void Poisson::Generate_Init_Object_Vector_(QRect insert_area_, MatrixXd Is_Boundary,Mat &source_image_)
{

	//Start position in the initial image
	int init_pos_x_ = insert_area_.left();
	int init_pos_y_ = insert_area_.top();

	//The width and length of the rectangular area
	int width = insert_area_.width();
	int height = insert_area_.height();
	int point_num_ = width * height;

	int width_image_ = source_image_.cols;
	int height_image_ = source_image_.rows;
	cout<<width_image_<<" "<<height_image_<<endl;
	cout<<width<<" " <<height<<endl;
	int channels_ = source_image_.channels();
	Object_.resize(point_num_,3);
	int i,j,index_;
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			index_ = j * width + i;
			if(abs(Is_Boundary(j,i))>eps)
			{
				Object_(index_,0) = -4 * source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[0];
				Object_(index_,1) = -4 * source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[1];
				Object_(index_,2) = -4 * source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[2];

				Object_(index_,0) = Object_(index_,0) + source_image_.at<Vec3b>(j+init_pos_y_-1,i+init_pos_x_)[0];
				Object_(index_,1) = Object_(index_,1) + source_image_.at<Vec3b>(j+init_pos_y_-1,i+init_pos_x_)[1];
				Object_(index_,2) = Object_(index_,2) + source_image_.at<Vec3b>(j+init_pos_y_-1,i+init_pos_x_)[2];

				Object_(index_,0) = Object_(index_,0) + source_image_.at<Vec3b>(j+init_pos_y_+1,i+init_pos_x_)[0];
				Object_(index_,1) = Object_(index_,1) + source_image_.at<Vec3b>(j+init_pos_y_+1,i+init_pos_x_)[1];
				Object_(index_,2) = Object_(index_,2) + source_image_.at<Vec3b>(j+init_pos_y_+1,i+init_pos_x_)[2];

				Object_(index_,0) = Object_(index_,0) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_-1)[0];
				Object_(index_,1) = Object_(index_,1) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_-1)[1];
				Object_(index_,2) = Object_(index_,2) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_-1)[2];

				Object_(index_,0) = Object_(index_,0) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_+1)[0];
				Object_(index_,1) = Object_(index_,1) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_+1)[1];
				Object_(index_,2) = Object_(index_,2) + source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_+1)[2];
				
			}
		}
		is_generate_vector_ = true;
		//cout<<Object_<<endl;
	/*	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			index_ = j * width + i;
			if(abs(Is_Boundary(j,i))>eps)
			{
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[0]=0;
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[1]=0;
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[2]=0;
			}
			else
			{
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[0]=255;
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[1]=255;
				source_image_.at<Vec3b>(j+init_pos_y_,i+init_pos_x_)[2]=255;
			}
		}*/
}


void Poisson::Poisson_Filling_(QRect insert_area_, MatrixXd Is_Boundary, Mat source_image_, Mat &target_image_, QPoint current_point)
{
	if(!is_generate_matrix_ )
		Generate_Coef_Matrix_(insert_area_,Is_Boundary);
	if(!is_generate_vector_)
		Generate_Init_Object_Vector_(insert_area_,Is_Boundary, source_image_);

	//Start position in the initial image
	int init_pos_x_ = insert_area_.left();
	int init_pos_y_ = insert_area_.top();

	//The width and length of the rectangular area
	int width = insert_area_.width();
	int height = insert_area_.height();
	int point_num_ = width * height;

	//Start position in the target image
	int targ_pos_x_ = current_point.rx() - width/2;
	int targ_pos_y_ = current_point.ry() - height/2;

	//Judge whether this area can be inserted in the the target image
	if(targ_pos_x_ < 0 || targ_pos_x_ + width > target_image_.cols || targ_pos_y_ < 0 || targ_pos_y_ + height > target_image_.rows)
	{
		cout<<"The chosen area is too big to filling!"<<endl;
		return;
	}
	MatrixXd Color_Matrix_(point_num_,3);	// Color(i*height+j,k) is R, G or B for point(i,j);

	int i,j,index_;
	uchar	*color;
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			if(abs(Is_Boundary(j,i))<eps)
			{
				index_ = j * width + i;

				Object_(index_,0) = target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[0];
				Object_(index_,1) = target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[1];
				Object_(index_,2) = target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[2];
			}

		}
	Color_Matrix_ = LU.solve(Object_);
	//Recoloring the target image
	for(j=0;j<height;j++)
		for(i=0;i<width;i++)
		{
			index_ = j * width + i;
			if(Color_Matrix_(index_,0) < 0)
				Color_Matrix_(index_,0) = 0;
			if(Color_Matrix_(index_,0) > 255)
				Color_Matrix_(index_,0) = 255;

			if(Color_Matrix_(index_,1) < 0)
				Color_Matrix_(index_,1) = 0;
			if(Color_Matrix_(index_,1) > 255)
				Color_Matrix_(index_,1) = 255;

			if(Color_Matrix_(index_,2) < 0)
				Color_Matrix_(index_,2) = 0;
			if(Color_Matrix_(index_,2) > 255)
				Color_Matrix_(index_,2) = 255;

			target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[0] = int(Color_Matrix_(index_,0));
			target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[1] = int(Color_Matrix_(index_,1));
			target_image_.at<Vec3b>(j+targ_pos_y_,i+targ_pos_x_)[2] = int(Color_Matrix_(index_,2));

		}

}