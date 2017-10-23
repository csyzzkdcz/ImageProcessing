#include"FigureTypeFreehand.h"

FigureTypeFreehand::FigureTypeFreehand(vector<QPoint> points_array_)
{
	int i,size_ = points_array_.size();
	for(i=0;i<size_;i++)
		Points_.push_back(points_array_[i]);
}

FigureTypeFreehand::~FigureTypeFreehand()
{
	y_max_ = y_min_ = 0;
	x_max_ = x_min_ = 0;
	m_vertices.clear();
	points_num_ = 0;
}

void FigureTypeFreehand::Get_Vertices()
{
	m_vertices.clear();
	int i,size_ = Points_.size();
	QPointF vertex_;
	for(i=0;i<size_;i++)
	{
		vertex_.setX(1.0* Points_[i].rx());
		vertex_.setY(1.0* Points_[i].ry());
		m_vertices.push_back(vertex_);
	}
}

void FigureTypeFreehand::Generate_BBox()
{
	BBox_ =  QRect(x_min_,y_min_,x_max_-x_min_+1,y_max_-y_min_+1);
}

void FigureTypeFreehand::Get_Y_Max()
{
	int i,size_=Points_.size();
	double max_ = Points_[0].ry();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(Points_[i].ry() > max_)
		{
			max_ = Points_[i].ry();
			index_ = i;
		}
	}
	y_max_ =  Points_[index_].ry();
}

void FigureTypeFreehand::Get_Y_Min()
{
	int i,size_=Points_.size();
	double min_ = Points_[0].ry();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(Points_[i].ry() < min_)
		{
			min_ = Points_[i].ry();
			index_ = i;
		}
	}
	y_min_ = Points_[index_].ry();
}

void FigureTypeFreehand::Get_X_Max()
{
	int i,size_=Points_.size();
	double max_ = Points_[0].rx();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(Points_[i].rx() > max_)
		{
			max_ = Points_[i].rx();
			index_ = i;
		}
	}
	x_max_ = Points_[index_].rx();
}

void FigureTypeFreehand::Get_X_Min()
{
	int i,size_=Points_.size();
	double min_ = Points_[0].rx();
	int index_ = 0;
	for(i=0;i<size_;i++)
	{
		if(Points_[i].rx() < min_)
		{
			min_ = Points_[i].rx();
			index_ = i;
		}
	}
	x_min_ = Points_[index_].rx();
}

void FigureTypeFreehand::Get_Points_Num()
{
	points_num_ = Points_.size();
}

void FigureTypeFreehand::Draw_Selected_Region_(QPainter &painter)
{
	int size_ = Points_.size();
	int i;
	for(i=0;i<size_-1;i++)
	{
		painter.drawLine(Points_[i],Points_[i+1]);
	}
	painter.drawLine(Points_[size_-1],Points_[0]);
}