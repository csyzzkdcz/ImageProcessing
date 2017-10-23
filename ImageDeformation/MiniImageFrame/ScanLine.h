#pragma once
#include<vector>
#include<list>
#include"Polygon.h"
#include <Eigen\Eigen>
#define eps 1e-10
using namespace Eigen;
using namespace std;

typedef struct EDGE
{
	double start_xi;				// present intersect point, the start point of this edge
	//double end_xi;					// present insert point, the end point of this edge
	double dx;						// 1/k
	double ymax;					// know that the minimal coordinate is on the top
	bool horizon_flag;				// whether this edge is horizon
}MY_EDGE;

typedef struct SCANELINE
{
	list<MY_EDGE> Net;				// store the intersected edge
}MY_SCAN_LINE;

class ScanLine
{
public:
	ScanLine();
	~ScanLine();
	void Scan_Line_Algorithm(PolygonCage *figure_);
	void Init_Figure(PolygonCage *figure);

public:
	QRect BBox;
	QPoint center_point_;
	MatrixXd Is_Boundary;
	vector<QPointF> my_vertices_;
	
private:
	
	void Init_ScanLine_Array();
	void InitNET(PolygonCage *figure);
	void InsertNet_to_Aet(int index_);			// index_ means the index_ th scanline
	void Set_Inner_Points_(int index_);			// set the inner points of the given area w.r.t. the scanline[index_]
	void UpdateAet();							// update AET from one scanline to the next;
	void DeleteNonActiveEdge(int index_);		// delete the nonactive edges
	bool IsEdgeOutOfActive(MY_EDGE edge_);
	void Exchange(MY_EDGE &edge_1, MY_EDGE &edge_2); // exchange all the information
	void Dealing_horizontal_EDGE();				//dealing with the horizontal edges

private:
	bool is_init_figure_;
	vector<MY_SCAN_LINE> scan_line_array_;
	list<MY_EDGE> AET;							// active edges for current scanline

public:
	int y_max_;
	int y_min_;
	int x_max_;
	int x_min_;
	int current_index_;			//use for delete

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;


};