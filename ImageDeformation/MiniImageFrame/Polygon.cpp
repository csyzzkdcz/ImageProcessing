#include"Polygon.h"

PolygonCage::PolygonCage()
{
	x_min_ = x_max_ = 0;
	y_min_ = y_max_ = 0;
	points_num_ = 0;
	vertices_.clear();
}

PolygonCage::~PolygonCage()
{
	x_min_ = x_max_ = 0;
	y_min_ = y_max_ = 0;
	points_num_ = 0;
	vertices_.clear();
}

PolygonCage::PolygonCage(vector<QPoint> vertices)
{
	int i;
	points_num_ = vertices.size();
	vertices_.clear();
	vertices_.resize(points_num_);
	for(i=0;i<points_num_;i++)
	{
		vertices_[i].setX(vertices[i].rx()*1.0);
		vertices_[i].setY(vertices[i].ry()*1.0);
	}
	Get_X_Min();
	Get_X_Max();
	Get_Y_Min();
	Get_Y_Max();
	BBox_ = QRect(x_min_,y_min_,x_max_-x_min_+1,y_max_-y_min_+1);

}

void PolygonCage::Get_Y_Max()
{
	int i,size_=points_num_;
	double max_ = vertices_[0].ry();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(vertices_[i].ry() > max_)
		{
			max_ = vertices_[i].ry();
			index_ = i;
		}
	}
	y_max_ =  vertices_[index_].ry();
}

void PolygonCage::Get_Y_Min()
{
	int i,size_=points_num_;
	double min_ = vertices_[0].ry();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(vertices_[i].ry() < min_)
		{
			min_ = vertices_[i].ry();
			index_ = i;
		}
	}
	y_min_ = vertices_[index_].ry();
}

void PolygonCage::Get_X_Max()
{
	int i,size_=points_num_;
	double max_ = vertices_[0].rx();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(vertices_[i].rx() > max_)
		{
			max_ = vertices_[i].rx();
			index_ = i;
		}
	}
	x_max_ = vertices_[index_].rx();
}

void PolygonCage::Get_X_Min()
{
	int i,size_=vertices_.size();
	double min_ = vertices_[0].rx();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(vertices_[i].rx() < min_)
		{
			min_ = vertices_[i].rx();
			index_ = i;
		}
	}
	x_min_ = vertices_[index_].rx();
}