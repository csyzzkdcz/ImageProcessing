#pragma once
#include <QWidget>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "ImageSegment.h"
#include "ImageSegment_Layer.h"
#include "ImageSegment_kMeans.h"

using namespace cv;

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

enum SegmentMethod
{
	kLayer_,
	kKmean_,
	kNone_
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

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();											// Save image to another file

	// Image processing
	void Invert();											// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();											// Restore image to origin
	void ChooseLayer();										// Choose Layer method to segment an image
	void Choosekmean();										// Choose kmean method to segment an image

private:
	void Segment();											// Segment the image

private:
	Mat										image_mat_;
	Mat										image_mat_backup_;
	SegmentMethod							method_;
	ImageSegment								*editor_;
};

