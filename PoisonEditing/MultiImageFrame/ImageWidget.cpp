#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <iostream>
#include "ChildWindow.h"

using std::cout;
using std::endl;
using namespace Eigen;

ImageWidget::ImageWidget(ChildWindow *relatewindow)
{

	Editor_ = new Poisson();
	scanline_ = new ScanLine();

	type_  = kDefault;

	draw_status_ = kNone;
	is_choosing_ = false;
	is_copied_ = false;
	is_pasting_ = false;
	finish_draw_ = false;
	is_first_press_ = true;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
	insert_area_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
	if(Editor_)
		delete(Editor_);
	if(scanline_)
		delete(scanline_);
	if(insert_area_)
		delete(insert_area_);
}

int ImageWidget::ImageWidth()
{
	//return image_->width();
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
	//return image_->height();
	return image_mat_.rows;
}

void ImageWidget::set_draw_status_to_choose_Rect()
{
	draw_status_ = kChoose;
	type_ = kRect;
}

void ImageWidget::set_draw_status_to_choose_Polygon()
{
	draw_status_ = kChoose;
	type_ = kPolygon;
}

void ImageWidget::set_draw_status_to_choose_Freehand()
{
	draw_status_ = kChoose;
	type_ = kFreehand;
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}


void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
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
	if(is_pasting_)
	{
		QImage image_show = QImage( (unsigned char *)(image_mat_temp_.data), image_mat_temp_.cols, image_mat_temp_.rows, image_mat_temp_.step, QImage::Format_RGB888 );
		QRect rect = QRect(0, 0, image_show.width(), image_show.height());
		painter.drawImage(rect, image_show);
	}
	else
	{
		QImage image_show = QImage( (unsigned char *)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888 );
		QRect rect = QRect(0, 0, image_show.width(), image_show.height());
		painter.drawImage(rect, image_show);
	}

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	switch(type_)
	{
	case kRect:
		painter.drawRect(point_start_.x(), point_start_.y(), 
		point_end_.x()-point_start_.x(), point_end_.y()-point_start_.y());
		break;
	case kPolygon:
		if(points_array_.size())
		{
			for(int i=0;i<points_array_.size()-1;i++)	//Draw the previous edges
				painter.drawLine(points_array_[i],points_array_[i+1]);
			if(finish_draw_)	//if drawing process is finished
				painter.drawLine(points_array_[points_array_.size()-1], points_array_[0]);
			else			//Draw current edges
				painter.drawLine(point_start_,point_end_);
		}
		break;
	case kFreehand:
		if(points_array_.size())
		{
			for(int i=0;i<points_array_.size()-1;i++)	//Draw the previous edges
				painter.drawLine(points_array_[i],points_array_[i+1]);	
			if(finish_draw_)	//if drawing process is finished
				painter.drawLine(points_array_[points_array_.size()-1], points_array_[0]);
			//painter.drawLine(point_start_,point_end_);//Draw current edges
		}
		break;
	}
	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent *mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kChoose:
			// Store point position for rectangle region
			is_choosing_ = true;
			point_start_ = point_end_ = mouseevent->pos();
			if(type_ == kPolygon && is_first_press_)
			{
				points_array_.clear();
				points_array_.push_back(point_start_);
				is_first_press_ = false;
				finish_draw_ = false;
			}
			if(type_ == kFreehand && is_first_press_)
			{
				points_array_.clear();
				points_array_.push_back(point_start_);
				is_first_press_ = false;
				finish_draw_ = false;
			}
			break;

		case kPaste:
		    // Paste rectangle region to object image
			{
				if( source_window_->imagewidget_->is_copied_ )
				{
					is_pasting_ = true;
					Paste(mouseevent->pos());
				}
			}
			
			update();
			break;

		default:
			break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent *mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			if(type_ == kFreehand)
				points_array_.push_back(point_end_);
		}
		break;

	case kPaste:
		// Paste rectangle region to object image
		if ( source_window_->imagewidget_->is_copied_ && is_pasting_ )
		{
			Paste( mouseevent->pos() );
		}

	default:
		break;
	}
	
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			switch(type_)
			{
			case kRect:
				point_end_ = mouseevent->pos();
				insert_area_ = new FigureTypeRect(point_start_,point_end_);
				is_choosing_ = false;
				draw_status_ = kNone;
				break;
			case kPolygon:
				if(Qt::LeftButton == mouseevent->button())
				{
					points_array_.push_back(point_end_);
					point_start_ = mouseevent->pos();
				}
				else if(draw_status_ == kChoose && Qt::RightButton == mouseevent->button()) //click right button to finish drawing
				{
					insert_area_ = new FigureTypePolygon(points_array_);
					finish_draw_ = true; 
					draw_status_ = kNone;
					is_first_press_ = true;
				}
				break;
			case kFreehand:
				if(Qt::LeftButton == mouseevent->button())
				{
					insert_area_ = new FigureTypeFreehand(points_array_);
					finish_draw_ = true; 
					draw_status_ = kNone;
					is_first_press_ = true;
					break;
				}
			}
		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
			image_mat_ = image_mat_temp_.clone();
		}

	default:
		break;
	}
	
	update();
}

void ImageWidget::Open(QString fileName)
{
	if (!fileName.isEmpty())
	{
		image_mat_ = cv::imread( fileName.toLatin1().data() );
		cvtColor( image_mat_, image_mat_, CV_BGR2RGB );	
		image_mat_backup_ = image_mat_.clone();
		image_mat_temp_ = image_mat_.clone();
	}

	update();
}


void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	Mat image_save;
	cvtColor(image_mat_, image_save, CV_RGB2BGR);
	imwrite(filename.toLatin1().data(), image_save);
}

void ImageWidget::Invert()
{
	MatIterator_<Vec3b> iter, iterend;
	for (iter=image_mat_.begin<Vec3b>(), iterend=image_mat_.end<Vec3b>(); iter != iterend; ++iter)
	{
		(*iter)[0] = 255-(*iter)[0];
		(*iter)[1] = 255-(*iter)[1];
		(*iter)[2] = 255-(*iter)[2];
	}

	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
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
					image_mat_.at<Vec3b>(j, i) = image_mat_backup_.at<Vec3b>(height-1-j, width-1-i);
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					image_mat_.at<Vec3b>(j, i) = image_mat_backup_.at<Vec3b>(j, width-1-i);
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
					image_mat_.at<Vec3b>(j, i) = image_mat_backup_.at<Vec3b>(height-1-j, i);
				}
			}
		}
	}

	update();
}


void ImageWidget::TurnGray()
{
	MatIterator_<Vec3b> iter, iterend;
	for (iter=image_mat_.begin<Vec3b>(), iterend=image_mat_.end<Vec3b>(); iter != iterend; ++iter)
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
	image_mat_ = image_mat_backup_.clone();
	update();
}

void ImageWidget::Prepare()
{
	if(insert_area_)
	{
		scanline_->Init_Figure(insert_area_);
		scanline_->Scan_Line_Algorithm(insert_area_);
		Editor_->Generate_Coef_Matrix_(scanline_->BBox,scanline_->Is_Boundary);
		Editor_->Generate_Init_Object_Vector_(scanline_->BBox,scanline_->Is_Boundary,image_mat_);
	}
}

void ImageWidget::Paste( QPoint center_point )
{
	if(source_window_->imagewidget_->Editor_)
	{
		image_mat_temp_ = image_mat_.clone();
		source_window_->imagewidget_->Editor_->Poisson_Filling_(source_window_->imagewidget_->scanline_->BBox,source_window_->imagewidget_->scanline_->Is_Boundary,source_window_->imagewidget_->image_mat_,image_mat_temp_,center_point);
	}
}