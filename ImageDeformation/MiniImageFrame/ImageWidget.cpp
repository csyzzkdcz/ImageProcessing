#pragma once
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ImageWidget.h"
using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	is_draw_poly_ = false;
	is_open_image_ = false;
	is_first_press_ = true;
	selected_index_ = INT_MAX;
	Option_ = kDefault;
	polygon_cage_ = NULL;
	cur_polygon_cage_ = NULL;
	Editor_ = NULL;
	scan_line_ = NULL;
	points_array_.clear();
}


ImageWidget::~ImageWidget(void)
{
	is_draw_poly_ = false;
	is_first_press_ = true;
	is_open_image_ = false;
	selected_index_ = INT_MAX;
	Option_ = kDefault;
	points_array_.clear();
	if(polygon_cage_)
		delete(polygon_cage_);
	if(cur_polygon_cage_)
		delete(cur_polygon_cage_);
	if(scan_line_)
		delete(scan_line_);
	if(Editor_)
		delete(Editor_);
}

double ImageWidget::dist(double point_start_x,double point_start_y,double point_end_x,double point_end_y)
{
	return sqrt(pow(point_end_x-point_start_x,2.0)+ pow(point_end_y-point_start_y,2.0));
}

void ImageWidget::set_draw_status_to_Draw_Polygon()
{
	is_draw_poly_ = true;
}

void ImageWidget::Set_WP()
{
	Option_ = kWP;
}

void ImageWidget::Set_DH()
{
	Option_ = kDH;
}

void ImageWidget::Set_MV()
{
	Option_ = kMV;
}

void ImageWidget::Draw_Point(QPainter &painter,QPoint point_,int flag)
{
	if(flag==0)
	{
		painter.setPen(QColor(Qt::green));
		painter.setBrush(QBrush(Qt::green));
	}
	else
	{
		painter.setPen(QColor(Qt::blue));
		painter.setBrush(QBrush(Qt::blue));
	}
    painter.drawEllipse(point_,POINT_SIZE,POINT_SIZE);
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QImage image_show = QImage( (unsigned char *)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888 );
	QRect rect = QRect(0, 0, image_show.width(), image_show.height());
	painter.drawImage(rect, image_show);

	// Draw Polygon cage
	//painter.setBrush(Qt::NoBrush);
	//painter.setPen(Qt::red);
	if(points_array_.size())
	{
		for(int i=0;i<points_array_.size();i++)		// Draw points
		{
			if(polygon_cage_ && selected_index_==i)
				Draw_Point(painter,points_array_[i],0);
			else
				Draw_Point(painter,points_array_[i],1);
		}
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::red);
		for(int i=0;i<points_array_.size()-1;i++)	//Draw the previous edges
		{
		//	Draw_Point(paintevent,points_array_[i]);
			painter.drawLine(points_array_[i],points_array_[i+1]);
		//	Draw_Point(paintevent,points_array_[i+1]);
		}
		if(finish_draw_)	//if drawing process is finished
			painter.drawLine(points_array_[points_array_.size()-1], points_array_[0]);
		else			//Draw current edges
			painter.drawLine(point_start_,point_end_);
	}
	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent *mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		// Store point position for rectangle region
		if(is_draw_poly_ && is_first_press_)
		{
			point_start_ = point_end_ = mouseevent->pos();
			points_array_.clear();
			points_array_.push_back(point_start_);
			is_first_press_ = false;
			finish_draw_ = false;
		}
		// Select point to move
		if(!is_draw_poly_ && polygon_cage_)
		{
			for(int i=0;i<points_array_.size();i++)
			{
				if(dist(points_array_[i].rx(),points_array_[i].ry(),mouseevent->pos().rx()*1.0,mouseevent->pos().ry()*1.0) < 2* POINT_SIZE)
				{
					selected_index_=i;
					break;
				}
			}
		}
	}
	update();
}

void ImageWidget::mouseMoveEvent(QMouseEvent *mouseevent)
{
	if(is_draw_poly_)
		point_end_ = mouseevent->pos();
	if(polygon_cage_ && selected_index_ != INT_MAX)
	{
		points_array_[selected_index_] = mouseevent->pos();
		Deformation_Editor();
	}
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *mouseevent)
{
	if(is_draw_poly_)
		if(Qt::LeftButton == mouseevent->button())
		{
			points_array_.push_back(point_end_);
			point_start_ = mouseevent->pos();
		}
		else if(Qt::RightButton == mouseevent->button()) //click right button to finish drawing
		{
			polygon_cage_ = new PolygonCage(points_array_);
			scan_line_ = new ScanLine();
			scan_line_->Scan_Line_Algorithm(polygon_cage_);
			finish_draw_ = true; 
			is_draw_poly_ = false;
			is_first_press_ = true;
		}
		if(polygon_cage_ && selected_index_ != INT_MAX)
		{
			selected_index_ = INT_MAX;
			if(Option_ != kDefault )
				Option_ = kDefault;
		}
	update();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		image_mat_ = cv::imread( fileName.toLatin1().data() );
		cvtColor( image_mat_, image_mat_, CV_BGR2RGB );	
		image_mat_backup_ = image_mat_.clone();
	}
	is_open_image_ = true;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	if(!is_open_image_)
		return;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	cv::Mat image_save;
	cvtColor(image_mat_, image_save, CV_RGB2BGR);
	imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	if(!is_open_image_)
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter=image_mat_.begin<cv::Vec3b>(), iterend=image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255-(*iter)[0];
		(*iter)[1] = 255-(*iter)[1];
		(*iter)[2] = 255-(*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	if(!is_open_image_)
		return;
	int width = image_mat_.cols;
	int height = image_mat_.rows;

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height-1-j, width-1-i);
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(j, width-1-i);
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					image_mat_.at<cv::Vec3b>(j, i) = image_mat_backup_.at<cv::Vec3b>(height-1-j, i);
				}
			}
		}
	}

	update();
}

void ImageWidget::TurnGray()
{
	if(!is_open_image_)
		return;
	cv::MatIterator_<cv::Vec3b> iter, iterend;
	for (iter=image_mat_.begin<cv::Vec3b>(), iterend=image_mat_.end<cv::Vec3b>(); iter != iterend; ++iter)
	{
		int itmp = ((*iter)[0]+(*iter)[1]+(*iter)[2])/3;
		(*iter)[0] = itmp;
		(*iter)[1] = itmp;
		(*iter)[2] = itmp;
	}

	update();
}

void ImageWidget::Restore()
{
	if(!is_open_image_)
		return;
	image_mat_ = image_mat_backup_.clone();
	for(int i =0; i< points_array_.size();i++ )					// return the polygon cage into the initial one
	{
		points_array_[i].setX(polygon_cage_->vertices_[i].rx());
		points_array_[i].setY(polygon_cage_->vertices_[i].ry());
	}
	update();
}

void ImageWidget::Deformation_Editor()
{
	if(!is_open_image_)
		return;
	cur_polygon_cage_ = new PolygonCage(points_array_);
	switch(Option_)
	{
		case kWP:
			Editor_ = new Image_Deformation_WP();
			Editor_->Deformation(image_mat_backup_,image_mat_,polygon_cage_,cur_polygon_cage_);
			break;
		case kMV:
			Editor_ = new Image_Deformation_MV();
			Editor_->Deformation(image_mat_backup_,image_mat_,polygon_cage_,cur_polygon_cage_);
			break;
		case kDH:
			Editor_ = new Image_Deformation_DH();
			Editor_->Deformation(image_mat_backup_,image_mat_,polygon_cage_,cur_polygon_cage_);
			break;
		case kDefault:
			break;
		}
		delete(cur_polygon_cage_);
		cur_polygon_cage_ = NULL;
}