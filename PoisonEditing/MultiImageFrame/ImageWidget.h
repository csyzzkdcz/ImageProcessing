#pragma once
#include <QWidget>
#include <Eigen\Sparse>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "Poisson.h"
#include "FigureType.h"
#include "FigureTypePolygon.h"
#include "FIgureTypeRect.h"
#include "FigureTypeFreehand.h"
#include "ScanLine.h"
class ChildWindow;
QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

using namespace Eigen;
using namespace cv;

enum DrawStatus
{
	kChoose, 
	kPaste, 
	kNone
};
enum Area_Type_
{
	kRect,
	kPolygon,
	kFreehand,
	kDefault,
};

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(ChildWindow *relatewindow);
	~ImageWidget(void);

	int ImageWidth();											// Width of image
	int ImageHeight();											// Height of image
	void set_draw_status_to_choose_Rect();
	void set_draw_status_to_choose_Polygon();
	void set_draw_status_to_choose_Freehand();
	void set_draw_status_to_paste();
	QImage* image();
	void set_source_window(ChildWindow* childwindow);
	void Prepare();
	// gain the chosen area

protected:
	void paintEvent(QPaintEvent *paintevent);
	void mousePressEvent(QMouseEvent *mouseevent);
	void mouseMoveEvent(QMouseEvent *mouseevent);
	void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
	// File IO
	void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void Paste(QPoint center_point );
	// Make source image accustomed to surroundings 

public:
	QPoint						point_start_;					// Left top point of rectangle region
	QPoint						point_end_;						// Right bottom point of rectangle region
	vector<QPoint>				points_array_;					// Poinrs array for freehand curve

private:

	Poisson						*Editor_;
	ScanLine					*scanline_;

	FigureType					*insert_area_;
	Area_Type_					type_;

	// Pointer of child window
	ChildWindow					*source_window_;				// Source child window

	// Signs
	DrawStatus					draw_status_;					// Enum type of draw status
	bool						is_choosing_;
	bool						is_pasting_;
	bool						finish_draw_;
	bool						is_first_press_;
public:
	bool						is_copied_;

public:
	Mat							image_mat_;
	Mat							image_mat_backup_;
	Mat							image_mat_temp_;

};

