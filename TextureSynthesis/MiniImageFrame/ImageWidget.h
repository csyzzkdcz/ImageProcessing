#pragma once
#include <QWidget>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include"Texture_Synthesis.h"

using namespace cv;

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

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
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void Choose_Random();
	void Choose_Entire();
	void Choose_Subpatch();
	void Is_DrawSeams();

private:
	void Do_Synthesis();

private:
	Mat										image_mat_;
	Mat										image_mat_backup_;
	Mat										image_seams_;
	Mat										image_mat_record_;
	Synthesis*								Editor_;
	Methods									method_type_;
	bool										Is_Draw_Seams_;
	bool										Is_done_;
};

