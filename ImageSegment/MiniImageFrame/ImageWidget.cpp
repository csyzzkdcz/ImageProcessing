#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	editor_ = NULL;
	method_ = kNone_;
}


ImageWidget::~ImageWidget(void)
{
	if (editor_)
		delete(editor_);
	editor_ = NULL;
	method_ = kNone_;
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

	painter.end();
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

void ImageWidget::ChooseLayer()
{
	method_ = kLayer_;
	Segment();
}

void ImageWidget::Choosekmean()
{
	method_ = kKmean_;
	Segment();
}

void ImageWidget::Segment()
{
	switch (method_)
	{
	case kLayer_:
		editor_ = new ImageSegment_Layer(image_mat_);
		editor_->DoSegment(image_mat_);
		update();
		break;
	case kKmean_:
		editor_ = new ImageSegment_kMeans(image_mat_);
		editor_->DoSegment(image_mat_);
		update();
		break;
	case kNone_:
		break;
	default:
		break;
	}
}