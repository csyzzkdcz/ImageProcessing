
#pragma once
#include <QWidget>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "Polygon.h"
#include "ScanLine.h"
#include "Image_Deformation.h"
#include "Image_Deformation_WP.h"
#include "Image_Deformation_MV.h"
#include "Image_Deformation_DH.h"


#define POINT_SIZE 3
//using namespace cv;

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE


enum Deform_Method
{
	kWP,
	kMV,
	kDH,
	kDefault,
};
class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);
	void Draw_Point(QPainter &painter,QPoint point_,int flag);		// Draw a point with a given size	

private:
	double dist(double point_start_x,double point_start_y,double point_end_x,double point_end_y);	// calculate the dist from start point to end point
	void   Deformation_Editor();								// Image deformatiion

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file


	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void set_draw_status_to_Draw_Polygon();						// Draw Polygon Cage

	// Image deformation processing
	void Set_WP();												// Use WP coordinates to warp a image
	void Set_MV();												// Use MV coordinates to warp a image
	void Set_DH();												// Use DH coordinates to warp a image

private:
	cv::Mat									image_mat_;
	cv::Mat									image_mat_backup_;
	vector<QPoint>							points_array_;
	QPoint									point_start_;
	QPoint									point_end_;
	bool									is_draw_poly_;
	bool									finish_draw_;
	bool									is_first_press_;
	bool									is_open_image_;
	PolygonCage								*polygon_cage_;
	PolygonCage								*cur_polygon_cage_;
	ScanLine								*scan_line_;
	Deform_Method							Option_;
	Image_Deformation						*Editor_;
	int										selected_index_;
};

