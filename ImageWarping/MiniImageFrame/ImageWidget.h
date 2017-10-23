#pragma once
#include <QWidget>
#include "Warping.h"
#include "Warping_IDW.h"
#include "Warping_RBF.h"

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
class QMouse;
QT_END_NAMESPACE

enum Warping_Method
{
	kDefault = 0,
	kIDW = 1,
	kRBF = 2,
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
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	

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
	void ImageWarping();
	void Set_Method_to_IDW_();									// Set warping method as IDW
	void Set_Method_to_RBF_();									// Set warping method as RBF
	void Is_Choose_Points_();										// Choose the transfer points


private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;
	
	Warping_Method		Method_;				// Warping method
	Warping				*warping_;
	bool		is_choose_points_;				//choose points
	bool		is_draw_line_;					//flag to charge whether a line should be drawn

	vector<QPoint> start_points_;				// the set of start points
	vector<QPoint> end_points_;					// the set of end points

	QPoint start_point_;
	QPoint end_point_;
};

