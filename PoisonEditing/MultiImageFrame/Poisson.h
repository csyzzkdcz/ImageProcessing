#pragma once
#include <Eigen>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <QImage>
#include <vector>
#include <iostream>
#include "FigureTypeRect.h"
#include"qrect.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
using namespace Eigen;
using namespace std;
using namespace cv;
class Poisson
{
public:
	Poisson();
	~Poisson();

public:
	void Poisson_Filling_(QRect insert_area_, MatrixXd Is_Boundary, Mat source_image_, Mat &target_image_, QPoint current_point_);
	void Generate_Coef_Matrix_(QRect insert_area_,MatrixXd Is_Boundary);	//Prepare for poisson editing
	void Generate_Init_Object_Vector_(QRect insert_area_, MatrixXd Is_Boundary,Mat &source_image_);//Genereate objective vector for linear system Ax = b


private:
	bool is_generate_matrix_;
	bool is_generate_vector_;
	MatrixXd Object_;	
	SparseLU <SparseMatrix<double>, COLAMDOrdering<int>>  LU;	//store the coefficient matrix as the formation of LU decomposition
	SimplicialCholesky <SparseMatrix <double>> LLT;
	SparseQR <SparseMatrix<double>, COLAMDOrdering<int>> QR;


public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};