#include "Triangulation.h"

Triangulation::Triangulation()
{
	scan_inner_ = NULL;
}

Triangulation::~Triangulation()
{
	if(scan_inner_)
		delete scan_inner_;
}

void Triangulation::generate_inner_points(PolygonCage *poly_cage_)
{
	vertices_.clear();
	inner_points_.clear();
	int i,size_ = poly_cage_->vertices_.size();
	for(i=0;i<size_;i++)
		vertices_.push_back( poly_cage_->vertices_[i]);
	scan_inner_ = new ScanLine();
	scan_inner_->Scan_Line_Algorithm(poly_cage_);
	int num_ = scan_inner_->BBox.width() * scan_inner_->BBox.height();
	num_ = 0.01 * num_ < MAX_PTS_NUM_ ? 0.01 * num_ : MAX_PTS_NUM_;
	int x,y,width,height;
	width = scan_inner_->BBox.width();
	height = scan_inner_->BBox.height();
	num_ = 4;
	for(i=0;i<num_;i++)
	{
		do
		{
			x = rand()%width;
			y = rand()%height;
		}while(scan_inner_->Is_Boundary(y,x)!=1);
		QPointF inner_point_(x+scan_inner_->x_min_,y+scan_inner_->y_min_);
		inner_points_.push_back(inner_point_);
	}
}

void Triangulation::Triangulation_with_rand(PolygonCage *poly_cage_, struct triangulateio &triangle_frame_)
	// triangulating with randomly given inner points without any refining process
{
	generate_inner_points(poly_cage_);
	struct triangulateio initial_frame_,vorout; 
	initial_frame_.numberofpoints = vertices_.size() + inner_points_.size();
	initial_frame_.numberofpointattributes = 0;
	initial_frame_.pointlist = (REAL *) malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.pointmarkerlist = (int * )malloc(initial_frame_.numberofpoints * sizeof(REAL));
	initial_frame_.pointattributelist = (REAL *) malloc(initial_frame_.numberofpoints *
                                         initial_frame_.numberofpointattributes *0*
                                          sizeof(REAL));
	initial_frame_.numberofregions = 0;
	initial_frame_.numberofsegments = vertices_.size();
	initial_frame_.numberofholes = 0;
	initial_frame_.regionlist = (REAL *) malloc(initial_frame_.numberofregions * 5 * sizeof(REAL));
	initial_frame_.segmentlist = (int * )malloc(vertices_.size() * 2 * sizeof(REAL));
	initial_frame_.segmentmarkerlist = (int * )malloc(vertices_.size() * sizeof(REAL));

	int i,j;
	for(i=0;i<vertices_.size();i++)
	{
		initial_frame_.pointlist[2*i] = vertices_[i].rx();
		initial_frame_.segmentlist[2*i] = i;
		initial_frame_.pointlist[2*i+1] = vertices_[i].ry();
		initial_frame_.segmentlist[2*i+1] = (i+1) % vertices_.size();
		initial_frame_.pointmarkerlist[i] = 1;
		initial_frame_.segmentmarkerlist[i] = 1;
	}
	for(i=vertices_.size();i<initial_frame_.numberofpoints;i++)
	{
		initial_frame_.pointlist[2*i] = inner_points_[i-vertices_.size()].rx();
		initial_frame_.pointlist[2*i+1] = inner_points_[i-vertices_.size()].ry();
		initial_frame_.pointmarkerlist[i] = 0;
	}
	triangle_frame_.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of point attributes is zero: */
	triangle_frame_.pointattributelist = (REAL *) NULL;
	triangle_frame_.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
	triangle_frame_.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
	triangle_frame_.triangleattributelist = (REAL *) NULL;
	triangle_frame_.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
  /* Needed only if segments are output (-p or -c) and -P not used: */
	triangle_frame_.segmentlist = (int *) NULL;
  /* Needed only if segments are output (-p or -c) and -P and -B not used: */
	triangle_frame_.segmentmarkerlist = (int *) NULL;
	triangle_frame_.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	triangle_frame_.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
  /* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	// Initial triangular frame
	triangulate("pzAevn", &initial_frame_, &triangle_frame_, &vorout);

	// free
	free(initial_frame_.pointlist);
  free(initial_frame_.pointattributelist);
  free(initial_frame_.pointmarkerlist);
  free(initial_frame_.regionlist);
  free(vorout.pointlist);
  free(vorout.pointattributelist);
  free(vorout.edgelist);
  free(vorout.normlist);


}

void Triangulation::Triangulation_with_area_constraint(PolygonCage *poly_cage_, double Min_area_, double Max_area_,struct triangulateio &triangle_frame_)
{
	vertices_.clear();
	int i,size_ = poly_cage_->vertices_.size();
	for(i=0;i<size_;i++)
		vertices_.push_back( poly_cage_->vertices_[i]);
	struct triangulateio initial_frame_,mid_frame_,vorout;
	initial_frame_.numberofpoints = vertices_.size();
	initial_frame_.numberofpointattributes = 0;
	initial_frame_.pointlist = (REAL *) malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.pointmarkerlist = (int * )malloc(initial_frame_.numberofpoints * sizeof(REAL));
	initial_frame_.pointattributelist = (REAL *) malloc(initial_frame_.numberofpoints *
                                         initial_frame_.numberofpointattributes *0*
                                          sizeof(REAL));
	initial_frame_.numberofregions = 0;
	initial_frame_.numberofsegments = initial_frame_.numberofpoints;
	initial_frame_.numberofholes = 0;
	initial_frame_.regionlist = (REAL *) malloc(initial_frame_.numberofregions * 5 * sizeof(REAL));
	initial_frame_.segmentlist = (int * )malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.segmentmarkerlist = (int * )malloc(initial_frame_.numberofpoints * sizeof(REAL));

	for(i=0;i<vertices_.size();i++)
	{
		initial_frame_.pointlist[2*i] = vertices_[i].rx();
		initial_frame_.segmentlist[2*i] = i;
		initial_frame_.pointlist[2*i+1] = vertices_[i].ry();
		initial_frame_.segmentlist[2*i+1] = (i+1) % vertices_.size();
		initial_frame_.pointmarkerlist[i] = 1;
		initial_frame_.segmentmarkerlist[i] = 1;
	}
	mid_frame_.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of point attributes is zero: */
	mid_frame_.pointattributelist = (REAL *) NULL;
	mid_frame_.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
	mid_frame_.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
	mid_frame_.triangleattributelist = (REAL *) NULL;
	mid_frame_.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
  /* Needed only if segments are output (-p or -c) and -P not used: */
	mid_frame_.segmentlist = (int *) NULL;
  /* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid_frame_.segmentmarkerlist = (int *) NULL;
	mid_frame_.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid_frame_.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
  /* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	// Initial triangular frame
	triangulate("pzAevn", &initial_frame_, &mid_frame_, &vorout);

	 /* Needed only if -r and -a switches used: */
  mid_frame_.trianglearealist = (REAL *) malloc(mid_frame_.numberoftriangles * sizeof(REAL));
  mid_frame_.trianglearealist[0] = Max_area_;
  mid_frame_.trianglearealist[1] = Min_area_;

  /* Make necessary initializations so that Triangle can return a */
  /*   triangulation in `out'.                                    */

  triangle_frame_.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of attributes is zero: */
  triangle_frame_.pointattributelist = (REAL *) NULL;
  triangle_frame_.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
  triangle_frame_.triangleattributelist = (REAL *) NULL;

  /* Refine the triangulation according to the attached */
  /*   triangle area constraints.                       */

  triangulate("prazBP", &mid_frame_, &triangle_frame_, (struct triangulateio *) NULL);

  //free
  free(initial_frame_.pointlist);
  free(initial_frame_.pointattributelist);
  free(initial_frame_.pointmarkerlist);
  free(initial_frame_.regionlist);
  free(mid_frame_.pointlist);
  free(mid_frame_.pointattributelist);
  free(mid_frame_.pointmarkerlist);
  free(mid_frame_.trianglelist);
  free(mid_frame_.triangleattributelist);
  free(mid_frame_.trianglearealist);
  free(mid_frame_.neighborlist);
  free(mid_frame_.segmentlist);
  free(mid_frame_.segmentmarkerlist);
  free(mid_frame_.edgelist);
  free(mid_frame_.edgemarkerlist);
  free(vorout.pointlist);
  free(vorout.pointattributelist);
  free(vorout.edgelist);
  free(vorout.normlist);

}

void Triangulation::Triangulation_with_angle_constraint(PolygonCage *poly_cage_,struct triangulateio &triangle_frame_)
{
		vertices_.clear();
	int i,size_ = poly_cage_->vertices_.size();
	for(i=0;i<size_;i++)
		vertices_.push_back( poly_cage_->vertices_[i]);
	struct triangulateio initial_frame_,mid_frame_,vorout;
	scan_inner_ = new ScanLine();
	scan_inner_->Scan_Line_Algorithm(poly_cage_);
	initial_frame_.numberofpoints = vertices_.size();
	initial_frame_.numberofpointattributes = 0;
	initial_frame_.pointlist = (REAL *) malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.pointmarkerlist = (int * )malloc(initial_frame_.numberofpoints * sizeof(REAL));
	initial_frame_.pointattributelist = (REAL *) malloc(initial_frame_.numberofpoints *
                                         initial_frame_.numberofpointattributes *0*
                                          sizeof(REAL));
	initial_frame_.numberofregions = 0;
	initial_frame_.numberofsegments = initial_frame_.numberofpoints;
	initial_frame_.numberofholes = 0;
	initial_frame_.regionlist = (REAL *) malloc(initial_frame_.numberofregions * 5 * sizeof(REAL));
	initial_frame_.segmentlist = (int * )malloc(initial_frame_.numberofpoints * 2 * sizeof(REAL));
	initial_frame_.segmentmarkerlist = (int * )malloc(initial_frame_.numberofpoints * sizeof(REAL));

	for(i=0;i<vertices_.size();i++)
	{
		initial_frame_.pointlist[2*i] = vertices_[i].rx();
		initial_frame_.segmentlist[2*i] = i;
		initial_frame_.pointlist[2*i+1] = vertices_[i].ry();
		initial_frame_.segmentlist[2*i+1] = (i+1) % vertices_.size();
		initial_frame_.pointmarkerlist[i] = 1;
		initial_frame_.segmentmarkerlist[i] = 1;
	}
	mid_frame_.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of point attributes is zero: */
	mid_frame_.pointattributelist = (REAL *) NULL;
	mid_frame_.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
	mid_frame_.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
	mid_frame_.triangleattributelist = (REAL *) NULL;
	mid_frame_.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
  /* Needed only if segments are output (-p or -c) and -P not used: */
	mid_frame_.segmentlist = (int *) NULL;
  /* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid_frame_.segmentmarkerlist = (int *) NULL;
	mid_frame_.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid_frame_.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
  /* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	// Initial triangular frame
	triangulate("pzAevn", &initial_frame_, &mid_frame_, &vorout);

	 /* Needed only if -r and -a switches used: */
  mid_frame_.trianglearealist = (REAL *) malloc(mid_frame_.numberoftriangles * sizeof(REAL));
  mid_frame_.trianglearealist[0] = 0;
  mid_frame_.trianglearealist[1] = 0;		// no area constraints

  /* Make necessary initializations so that Triangle can return a */
  /*   triangulation in `out'.                                    */

  triangle_frame_.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
  /* Not needed if -N switch used or number of attributes is zero: */
  triangle_frame_.pointattributelist = (REAL *) NULL;
  triangle_frame_.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
  /* Not needed if -E switch used or number of triangle attributes is zero: */
  triangle_frame_.triangleattributelist = (REAL *) NULL;

  /* Refine the triangulation according to the attached */
  /*   triangle area constraints.                       */

  triangulate("prazBPa30", &mid_frame_, &triangle_frame_, (struct triangulateio *) NULL);

  // free 
  free(initial_frame_.pointlist);
  free(initial_frame_.pointattributelist);
  free(initial_frame_.pointmarkerlist);
  free(initial_frame_.regionlist);
  free(mid_frame_.pointlist);
  free(mid_frame_.pointattributelist);
  free(mid_frame_.pointmarkerlist);
  free(mid_frame_.trianglelist);
  free(mid_frame_.triangleattributelist);
  free(mid_frame_.trianglearealist);
  free(mid_frame_.neighborlist);
  free(mid_frame_.segmentlist);
  free(mid_frame_.segmentmarkerlist);
  free(mid_frame_.edgelist);
  free(mid_frame_.edgemarkerlist);
  free(vorout.pointlist);
  free(vorout.pointattributelist);
  free(vorout.edgelist);
  free(vorout.normlist);
}