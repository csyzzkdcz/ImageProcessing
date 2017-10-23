#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <Eigen\Sparse>
#include "ui_mainwindow.h"

class ChildWindow;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE
	
using namespace Eigen;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	// File IO
	void Open();								// Open an existing file
	void Save();								// Save image to file
	void SaveAs();
	ChildWindow *CreateChildWindow();
	void SetActiveSubWindow(QWidget* window);

	// Image Processing
	void GrayScale();							// Turn image to gray-scale map
	void Restore();								// Restore image to origin
	void Invert();								// Invert each pixel's rgb value
	void Mirror();								// Mirror image vertically or horizontally


	// Poisson Image Editing
	void ChooseRect();							// Choose rectangle region
	void ChoosePolygon();						// Choose closed Polygon region
	void ChooseFreehand();						// Choose closed Freehand region
	void Copy();							    // Copy image in the chosen region
	void Paste();								// Paste rect region to object image

private:
	void CreateActions();
	void CreateMenus();
	void CreateToolBars();
	void CreateStatusBar();

	QMdiSubWindow *FindChild(const QString &filename);
	
private:
	Ui::MainWindowClass ui;

	QMenu						*menu_file_;
	QMenu						*menu_edit_;
	QMenu						*menu_help_;
	QToolBar					*toolbar_file_;
	QAction						*action_open_;
	QAction						*action_save_;
	QAction						*action_saveas_;

	QAction						*action_invert_;
	QAction						*action_mirror_;
	QAction						*action_gray_;
	QAction						*action_restore_;


	QAction						*action_choose_rect_;
	QAction						*action_choose_polygon_;
	QAction						*action_choose_freehand_;
	QAction						*action_copy_;
	QAction						*action_paste_;

	QMdiArea					*mdi_area_;
	QSignalMapper				*window_mapper_;

	ChildWindow					*child_source_;
};

#endif // MAINWINDOW_H
