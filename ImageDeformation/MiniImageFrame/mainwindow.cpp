#include "mainwindow.h"
#include <QtWidgets>
#include <QImage>
#include <QPainter>
#include "ImageWidget.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	//ui.setupUi(this);

	setGeometry(300, 150, 800, 450);

	imagewidget_ = new ImageWidget();
	setCentralWidget(imagewidget_);

	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *e)
{

}

void MainWindow::paintEvent(QPaintEvent* paintevent)
{
	
}

void MainWindow::CreateActions()
{
	action_new_ = new QAction(QIcon(":/MainWindow/Resources/images/new.png"), tr("&New"), this);
	action_new_->setShortcut(QKeySequence::New);
	action_new_->setStatusTip(tr("Create a new file"));

	action_open_ = new QAction(QIcon(":/MainWindow/Resources/images/open.png"), tr("&Open..."), this);
	action_open_->setShortcuts(QKeySequence::Open);
	action_open_->setStatusTip(tr("Open an existing file"));
	connect(action_open_, SIGNAL(triggered()), imagewidget_, SLOT(Open()));

	action_save_ = new QAction(QIcon(":/MainWindow/Resources/images/save.png"), tr("&Save"), this);
	action_save_->setShortcuts(QKeySequence::Save);
	action_save_->setStatusTip(tr("Save the document to disk"));

	action_saveas_ = new QAction(tr("Save &As..."), this);
	action_saveas_->setShortcuts(QKeySequence::SaveAs);
	action_saveas_->setStatusTip(tr("Save the document under a new name"));
	connect(action_saveas_, SIGNAL(triggered()), imagewidget_, SLOT(SaveAs()));

	action_invert_ = new QAction(tr("Inverse"), this);
	action_invert_->setStatusTip(tr("Invert all pixel value in the image"));
	connect(action_invert_, SIGNAL(triggered()), imagewidget_, SLOT(Invert()));

	action_mirror_ = new QAction(tr("Mirror"), this);
	action_mirror_->setStatusTip(tr("Mirror image vertically or horizontally"));
	connect(action_mirror_, SIGNAL(triggered()), imagewidget_, SLOT(Mirror()));

	action_gray_ = new QAction(tr("Grayscale"), this);
	action_gray_->setStatusTip(tr("Gray-scale map"));
	connect(action_gray_, SIGNAL(triggered()), imagewidget_, SLOT(TurnGray()));

	action_restore_ = new QAction(tr("Restore"), this);
	action_restore_->setStatusTip(tr("Show origin image"));
	connect(action_restore_, SIGNAL(triggered()), imagewidget_, SLOT(Restore()));

	action_poly_cage_ = new QAction(QIcon(":/MainWindow/Resources/images/polygon.png"), tr("Draw Polygon Cage"), this);
	connect(action_poly_cage_,SIGNAL(triggered()), imagewidget_, SLOT(set_draw_status_to_Draw_Polygon()));

	action_WP_ = new QAction(tr("WP"),this);
	action_WP_ ->setStatusTip(tr("Use WP coordinates to deform a image"));
	connect(action_WP_, SIGNAL(triggered()), imagewidget_, SLOT(Set_WP()));

	action_MV_ = new QAction(tr("MV"),this);
	action_MV_ ->setStatusTip(tr("Use MV coordinates to deform a image"));
	connect(action_MV_, SIGNAL(triggered()), imagewidget_, SLOT(Set_MV()));

	action_DH_ = new QAction(tr("DH"),this);
	action_DH_ ->setStatusTip(tr("Use DH coordinates to deform a image"));
	connect(action_DH_, SIGNAL(triggered()), imagewidget_, SLOT(Set_DH()));
}

void MainWindow::CreateMenus()
{
	menu_file_ = menuBar()->addMenu(tr("&File"));
	menu_file_->setStatusTip(tr("File menu"));
	menu_file_->addAction(action_new_);
	menu_file_->addAction(action_open_);
	menu_file_->addAction(action_save_);
	menu_file_->addAction(action_saveas_);

	menu_edit_ = menuBar()->addMenu(tr("&Edit"));
	menu_edit_->setStatusTip(tr("Edit menu"));
	menu_edit_->addAction(action_invert_);
	menu_edit_->addAction(action_mirror_);
	menu_edit_->addAction(action_gray_);
	menu_edit_->addAction(action_restore_);
	menu_edit_->addAction(action_poly_cage_);
	menu_edit_->addAction(action_WP_); 
	menu_edit_->addAction(action_MV_); 
	menu_edit_->addAction(action_DH_); 
}

void MainWindow::CreateToolBars()
{
	toolbar_file_ = addToolBar(tr("File"));
	toolbar_file_->addAction(action_new_);
	toolbar_file_->addAction(action_open_);
	toolbar_file_->addAction(action_save_);

	// Add separator in toolbar 
	toolbar_file_->addSeparator();
	toolbar_file_->addAction(action_invert_);
	toolbar_file_->addAction(action_mirror_);
	toolbar_file_->addAction(action_gray_);
	toolbar_file_->addAction(action_restore_);
	toolbar_file_->addAction(action_poly_cage_);
	toolbar_file_->addAction(action_WP_);
	toolbar_file_->addAction(action_MV_);
	toolbar_file_->addAction(action_DH_);
}

void MainWindow::CreateStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}



