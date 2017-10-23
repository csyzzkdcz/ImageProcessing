#include"FigureTypeRect.h"

FigureTypeRect::FigureTypeRect()
{

}

FigureTypeRect::~FigureTypeRect()
{
	y_max_ = y_min_ = 0;
	x_max_ = x_min_ = 0;
	m_vertices.clear();
	points_num_ = 0;
}

FigureTypeRect::FigureTypeRect(QPoint start_point_, QPoint end_point_)
{
	int rx = start_point_.rx() < end_point_.rx() ? start_point_.rx() : end_point_.rx();
	int ry = start_point_.ry() < end_point_.ry() ? start_point_.ry() : end_point_.ry();
	Left_Bottom.setX(rx);
	Left_Bottom.setY(ry);
	rx = start_point_.rx() > end_point_.rx() ? start_point_.rx() : end_point_.rx();
	ry = start_point_.ry() > end_point_.ry() ? start_point_.ry() : end_point_.ry();
	Right_Up.setX(rx);
	Right_Up.setY(ry);
	width = abs(end_point_.rx() - start_point_.rx()) + 1;
	height = abs(end_point_.ry() - start_point_.ry()) + 1;
	point_start_ = start_point_;
	point_end_ = end_point_;
}

void FigureTypeRect::Get_Vertices()
{
	m_vertices.clear();
	QPointF vertex_;
	vertex_.setX(Left_Bottom.rx()*1.0);
	vertex_.setY(Left_Bottom.ry()*1.0);
	m_vertices.push_back(vertex_);
	vertex_.setY(1.0*Right_Up.ry());
	m_vertices.push_back(vertex_);
	vertex_.setX(1.0* Right_Up.rx());
	m_vertices.push_back(vertex_);
	vertex_.setY (1.0*Left_Bottom.ry());
	m_vertices.push_back(vertex_);

}

void FigureTypeRect::Generate_BBox()
{
	BBox_ = QRect(x_min_,y_min_,x_max_-x_min_+1,y_max_-y_min_+1);
}

void FigureTypeRect::Get_Y_Max()
{
	y_max_ = Right_Up.ry();
}

void FigureTypeRect::Get_Y_Min()
{
	y_min_ =  Left_Bottom.ry();
}

void FigureTypeRect::Get_X_Max()
{
	x_max_ =  Right_Up.rx();
}

void FigureTypeRect::Get_X_Min()
{
	x_min_ =  Left_Bottom.rx();
}

void FigureTypeRect::Get_Points_Num()
{
	points_num_ = 4;
}

void FigureTypeRect::Draw_Selected_Region_(QPainter &painter)
{
	painter.drawRect(point_start_.rx(),point_start_.ry(),point_end_.rx()-point_start_.rx(),point_end_.ry()-point_end_.rx());
}

int FigureTypeRect::Get_Width_()
{
	return width;
}

int FigureTypeRect::Get_Height_()
{
	return height;
}

QPoint FigureTypeRect::Get_Left_Button_()
{
	return Left_Bottom;
}

QPoint FigureTypeRect::Get_Right_Up_()
{
	return Right_Up;
}

bool FigureTypeRect::Is_Boundary(int i,int j)
{
	bool judge_ = false;
	if(i == Left_Bottom.rx() || i == Right_Up.rx()||j == Left_Bottom.ry() || j == Right_Up.ry())
			judge_ = true;
	return judge_;
}