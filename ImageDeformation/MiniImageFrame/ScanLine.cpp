#include"ScanLine.h"
#include<iostream>
using namespace std;
ScanLine::ScanLine()
{
	is_init_figure_ = false;
}

ScanLine::~ScanLine()
{
	is_init_figure_ = false;
}

void ScanLine::Init_ScanLine_Array()		// After initing figure
{
	for(int i=y_min_;i<=y_max_;i++)
	{
		MY_SCAN_LINE temp_;
		scan_line_array_.push_back(temp_);
	}
}

void ScanLine::Init_Figure(PolygonCage *figure)
{

	BBox = figure->BBox_;
	center_point_ = BBox.center();
	my_vertices_.clear();
	AET.clear();
	scan_line_array_.clear();
	Is_Boundary.setZero(figure->y_max_-figure->y_min_+1,figure->x_max_-figure->x_min_+1);
	y_max_ = figure->y_max_;
	y_min_ = figure->y_min_;
	x_max_ = figure->x_max_;
	x_min_ = figure->x_min_;
	int i,size_ = figure->points_num_;
	for(i=0;i<size_;i++)
		my_vertices_.push_back(figure->vertices_[i]);
	Init_ScanLine_Array();
	is_init_figure_ = true;
}

void ScanLine::InitNET(PolygonCage *figure)
{
	if(!is_init_figure_)
		Init_Figure(figure);
	MY_EDGE temp_edge_;
	int i;
	for(i=0;i<figure->points_num_;i++)
	{
		QPointF start_point_ = my_vertices_[i];
		QPointF end_point_= my_vertices_[(i+1)%figure->points_num_];
		QPointF before_start_point_ = my_vertices_[(i-1+figure->points_num_)%figure->points_num_];
		QPointF after_end_point_ = my_vertices_[(i+2)%figure->points_num_];
		if(abs(start_point_.ry()-end_point_.ry())>eps)		// disgard the horizontal lines
		{
			temp_edge_.horizon_flag = false;
			temp_edge_.dx = (start_point_.rx()-end_point_.rx())/(start_point_.ry()-end_point_.ry());
			if(end_point_.ry() > start_point_.ry())
			{
				temp_edge_.start_xi = start_point_.rx();
				if(after_end_point_.ry() >= end_point_.ry())		// for the case of the left vertex
					temp_edge_.ymax = end_point_.ry()-1;
				else
					temp_edge_.ymax = end_point_.ry();
				scan_line_array_[int(start_point_.ry())-figure->y_min_].Net.push_front(temp_edge_);
			}
			else
			{
				temp_edge_.start_xi = end_point_.rx();
				if(before_start_point_.ry() >= start_point_.ry())	// for the case of the right vertex
					temp_edge_.ymax = start_point_.ry()-1;
				else
					temp_edge_.ymax = start_point_.ry();
				scan_line_array_[int(end_point_.ry())-figure->y_min_].Net.push_front(temp_edge_);
			}
		}
	}

}

void ScanLine::Scan_Line_Algorithm(PolygonCage *figure)
{
	InitNET(figure);
	for(int i=y_min_;i<=y_max_;i++)
	{
		InsertNet_to_Aet(i-y_min_);      // transfer Net to AET for each scanline
		Set_Inner_Points_(i-y_min_);
		current_index_ = i-y_min_;
		DeleteNonActiveEdge(i-y_min_);
		UpdateAet();					// update scanline from the current to the next
	}
	Dealing_horizontal_EDGE();

}

void ScanLine::Dealing_horizontal_EDGE()
{
	int i=0;
	int size_ = my_vertices_.size();
	for(i=0;i<size_;i++)
	{
		QPointF start_point_ = my_vertices_[i];
		QPointF end_point_= my_vertices_[(i+1)%size_];
		if(abs(end_point_.ry()-start_point_.ry())<eps)
		{
			int index_i = int(end_point_.ry());
			int start_j = int(start_point_.rx()) < int(end_point_.rx()) ? start_point_.rx():end_point_.rx();
			int end_j = int(start_point_.rx()) > int(end_point_.rx()) ? start_point_.rx():end_point_.rx();
			for(int j=start_j;j<=end_j;j++)
				Is_Boundary(index_i-y_min_,j-x_min_)=0;
		}

	}
}

void ScanLine::InsertNet_to_Aet(int index_)	//Transfer Net to the AET for the scanline[index_]
	// We do not need to consider the horizontal situation for it has been predealt
{
	list <MY_EDGE>::iterator itr,itr_AET;
	itr=scan_line_array_[index_].Net.begin();
	for(;itr!=scan_line_array_[index_].Net.end();itr++)
		//insert
	{
		if(AET.empty())
			AET.push_back((*itr));
		else
		{
			itr_AET = AET.begin();
			bool is_insert_ = false;
			for(;itr_AET!=AET.end();itr_AET++)
			{
				if((*itr).start_xi < (*itr_AET).start_xi)
				{
					AET.insert(itr_AET,(*itr));
					is_insert_ = true;
					break;
				}
				if(abs((*itr).start_xi - (*itr_AET).start_xi) < eps)
				{
					if((*itr).dx < (*itr_AET).dx)
					{
						AET.insert(itr_AET,(*itr));
						is_insert_ = true;
						break;
					}
					else if((*itr).dx > (*itr_AET).dx)
					{
						AET.insert(++itr_AET,(*itr));
						is_insert_ = true;
						break;
					}

				}
			}
			// if it is larger than all of the current edge, push back it
			if(!is_insert_)
				AET.push_back((*itr));
		}

	}
}

void ScanLine::Set_Inner_Points_(int index_)
{
	list<MY_EDGE>:: iterator itr;
	for(itr = AET.begin();itr!=AET.end();)
	{
		int start_ = (*itr).start_xi;
		if((*itr).start_xi -start_< eps )
			Is_Boundary(index_, start_ - x_min_) = -1;  // on the edge
		itr++;
		int end_ = (*itr).start_xi;
		if((*itr).start_xi - end_ < eps)
		{
			Is_Boundary(index_, end_ - x_min_) = -1;	// on the edge
			end_--;
		}
		for(int i =start_+1;i<=end_;i++)
		{
			Is_Boundary(index_,i-x_min_) = 1;
		}
		itr++;
	}
}

void ScanLine::UpdateAet()
{
	list<MY_EDGE>::iterator itr,temp_itr_,itr_min_;
	for(itr = AET.begin();itr!=AET.end();itr++)
	{
		(*itr).start_xi = (*itr).start_xi+(*itr).dx;
	}
	//resort
	for(itr = AET.begin();itr!=AET.end();itr++)
	{
		itr_min_ = itr;
		for(temp_itr_=itr;temp_itr_!=AET.end();temp_itr_++)
		{
			if((*itr_min_).start_xi > (*temp_itr_).start_xi)
			{
				itr_min_ = temp_itr_;
			}
		}
		Exchange((*itr),(*itr_min_));
	}
	
}

void ScanLine::Exchange(MY_EDGE &edge_1, MY_EDGE & edge_2)
{
	double temp_xi, temp_dx,temp_ymax_,temp_bool_;
	temp_xi = edge_1.start_xi;
	edge_1.start_xi = edge_2.start_xi;
	edge_2.start_xi = temp_xi;

	temp_dx = edge_1.dx;
	edge_1.dx = edge_2.dx;
	edge_2.dx = temp_dx;

	temp_ymax_ = edge_1.ymax;
	edge_1.ymax = edge_2.ymax;
	edge_2.ymax = temp_ymax_;

	temp_bool_ = edge_1.horizon_flag;
	edge_1.horizon_flag = edge_2.horizon_flag;
	edge_2.horizon_flag = temp_bool_;

}

bool ScanLine::IsEdgeOutOfActive(MY_EDGE edge_)
{
	return ( abs(current_index_-edge_.ymax)<eps); 
}

void ScanLine::DeleteNonActiveEdge(int index_)
{
	std::list <EDGE>::iterator itr = AET.begin();
	for (;itr != AET.end();)
	{
		if (abs((*itr).ymax - index_-y_min_)<eps)
		{								
			itr = AET.erase(itr++);
			continue;
		}
		else
			itr++;
	}
}