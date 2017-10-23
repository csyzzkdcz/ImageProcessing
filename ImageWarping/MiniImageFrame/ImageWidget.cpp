#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	Method_ = kDefault;
	is_choose_points_ = false;
	is_draw_line_ = false;
}


ImageWidget::~ImageWidget(void)
{
	Method_ = kDefault;
	is_choose_points_ = false;
	if(warping_)
		delete warping_;
}

void ImageWidget::Set_Method_to_IDW_()
{
	Method_ = kIDW;
	ImageWarping();
}

void ImageWidget::Set_Method_to_RBF_()
{
	Method_ = kRBF;
	ImageWarping();
}

void ImageWidget::Is_Choose_Points_()
{
	is_choose_points_ = true;
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
	if(ptr_image_ && is_choose_points_ && Qt::LeftButton == event->button() && ptr_image_->rect().contains(event->pos()))
	{
		start_point_ = end_point_ = event->pos();
		is_draw_line_ = true;
		cout<<start_point_.rx()<<" ";
		cout<<end_point_.ry()<<endl;
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	setMouseTracking(true);
	if(is_draw_line_)
		end_point_ = event->pos();
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(is_draw_line_ && Qt::LeftButton == event->button())
	{
		start_points_.push_back(start_point_);
		end_point_ = event->pos();
		end_points_.push_back(end_point_);
		is_draw_line_ = false;
		cout<<end_point_.rx()<<" "<<end_point_.ry()<<endl;
	}
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
	QRect rect = QRect( 0, 0, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_);

	//Draw constraint lines
	if(is_choose_points_)
	{
		QPen pen(Qt::red, 2);
		painter.setPen(pen);
		painter.drawLine(start_point_,end_point_);
		for(int i = 0;i<start_points_.size();i++)
			painter.drawLine(start_points_[i],end_points_[i]);
	}
	painter.end();
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
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

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
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
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
	}
	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	start_points_.clear();
	end_points_.clear();
	start_point_.setX(0);
	start_point_.setY(0);
	end_point_ = start_point_;
	update();
}

void ImageWidget::ImageWarping()
{
	//Image warping
	switch(Method_)
	{
	case kDefault:
		break;
	case kIDW:
		warping_=new Warping_IDW();
		is_choose_points_ = false;
		Method_ = kDefault;
		break;
	case kRBF:
		warping_=new Warping_RBF();
		is_choose_points_ = false;
		Method_ = kDefault;
		break;
	}
	warping_->InitControlPoints(start_points_,end_points_);
	start_points_.clear();
	end_points_.clear();
	start_point_.setX(0);
	start_point_.setY(0);
	end_point_ = start_point_;
	warping_->DoWarping_(*ptr_image_);
	update();
}