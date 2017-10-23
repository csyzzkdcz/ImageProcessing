#pragma once
#include"Reference.h"
#include"qimage.h"
#include<vector>
#include<qpoint.h>
#include<cmath>
#include<Eigen>
#include <ANN/ANN.h>					// ANN declarations
using namespace std;
using namespace Eigen;

class Warping
{
public:
	Warping();
	virtual ~Warping(){};
	virtual void DoWarping_(QImage &image_){};
	void InitControlPoints(vector<QPoint> points_start_,vector<QPoint> points_end_);

protected:
	double calculate_sum_(vector<double> Array);
	double distance(QPointF start_point_, QPointF end_point_);
	void  SetFixedPoints(QImage &image);		// The four points in the corner are the fixed points
	void  Fill_White_Holes(QImage &image);		// Fill the white holes
	int Find_Nearest_Int_(double x);			// Find the nearest integer for x
	void  Build_kdTree(QImage image_);			// Build kdTree
	
protected:
	void  Find_Adjacent_Points_NeighBor_(int pos_x_,int pos_y_,int width_,int height_);	// Find the adjacent points of a point in a given neighborhood
	void  Find_Adjacent_Points_ANN_(int pos_x_, int pos_y_, int num_,int width_, int height_);	// Find the neerest points with the number of k of a point

protected: // Interpolating methods
	void  Interpolating_IDW(int pos_x_,int pos_y_, double &red_,double &green_, double &blue_); //applying IDW algorithm to interpolate the white hole
	void  Interpolating_Ave(int pos_x_,int pos_y_, double &red_,double &green_, double &blue_); // interpolating color directly with average
	void  Interpolating_IDW_Ave(int pos_x_,int pos_y_, double &red_,double &green_, double &blue_); //interpolating color with IDW but fi(p) = color_i permanently
	void  Interpolating_RBF(int pos_x_, int pos_y_, double &red_,double &green_, double &blue_);	// applying RBF algorithm to interpolate the white hole

protected:
	vector<QPointF> start_points_;
	vector<QPointF> end_points_;
	MatrixXi		Is_Colored_;					// This matrix is used to store whether a pixel is colored,1 represents the pixel has been colored,0 represents not 
	vector<QPointF>	Adjacent_Points_;				// This vector store the information of adjacent points
	vector<double>	Red_;
	vector<double>  Green_;
	vector<double>	Blue_;

private:
	ANNkd_tree*			kdTree;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
};