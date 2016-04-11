#include "snakeFilter.h"

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

// -------------------------------------------------------------------------
SnakeFilter* SnakeFilter::
New( ) {
  return( new SnakeFilter( ) );
}

// -------------------------------------------------------------------------
SnakeFilter::
SnakeFilter( ) : vtkPolyDataAlgorithm( ) {}

// -------------------------------------------------------------------------
SnakeFilter::~SnakeFilter( ) {}
// -------------------------------------------------------------------------
int SnakeFilter::
RequestData(
    vtkInformation* request,
    vtkInformationVector** input,
    vtkInformationVector* output
    )
{

    // Get input
    vtkInformation* input_info = input[ 0 ]->GetInformationObject( 0 );
    vtkPolyData* in = vtkPolyData::SafeDownCast(
        input_info->Get( vtkDataObject::DATA_OBJECT( ) )
        );
    vtkPoints* in_points = in->GetPoints( );

    // Get output
    vtkInformation* output_info = output->GetInformationObject( 0 );
    vtkPolyData* out = vtkPolyData::SafeDownCast(
        output_info->Get( vtkDataObject::DATA_OBJECT( ) )
        );

    // Real output objects
    vtkPoints* out_points = vtkPoints::New( );
    vtkCellArray* out_lines = vtkCellArray::New( );
    vtkCellArray* out_verts = vtkCellArray::New( );

    double* currP;
    int N = in_points->GetNumberOfPoints( );
    std::cout << "number of points " << N << std::endl;
    for(unsigned long i = 0; i < N; i++) {
        currP = in_points->GetPoint(i);
        std::cout << "curr " << currP[0] << " " << currP[1] << std::endl;
        //TODO add image and external forces
        currP[0] += internalForce_x(i, in_points);
        currP[1] += internalForce_y(i, in_points);

        out_points->InsertNextPoint(currP);
    }

    for( unsigned int i = 0; i < N - 1; ++i ) {
        out_verts->InsertNextCell( 1 );
        out_verts->InsertCellPoint( i );

        out_lines->InsertNextCell( 2 );
        out_lines->InsertCellPoint( i );
        out_lines->InsertCellPoint( i+1 );

    }
    out_lines->InsertNextCell( 2 );
    out_lines->InsertCellPoint( (unsigned int)(N-1) );
    out_lines->InsertCellPoint( (unsigned int)0 );


    out->SetPoints( out_points );
    out->SetLines( out_lines );
    out->SetVerts( out_verts );

}

SnakeFilter::SnakeFilter(std::vector<Point> _points, double _tension, double _stiffness) :
                    points(_points), tension(_tension), stiffness(_stiffness) {
    movedPoints.assign(points.size(), Point());
}

SnakeFilter::SnakeFilter(std::vector<Point> _points, double _tension, double _stiffness,
                double _line_weight, double _edge_weight, double _term_weight) :
                    points(_points), tension(_tension), stiffness(_stiffness),
                    line_weight(_line_weight), edge_weight(_edge_weight),
                    term_weight(_term_weight) {

    movedPoints.assign(points.size(), Point());
}

std::vector<Point> SnakeFilter::getPoints() {
    return points;
}
/*
void SnakeFilter::update() {
    for(int i = 0; i < points.size(); i++) {
        //TODO add image and external forces
        movedPoints[i].x = points[i].x + internalForce_x(i);
        movedPoints[i].y = points[i].y + internalForce_y(i);
    }
    points = movedPoints;
}
*/
double SnakeFilter::internalForce_x(int i, vtkPoints* in_points) {
    return  tension   * continuityForce_x(i, in_points) +
            stiffness * curvatureForce_x(i, in_points);
}

double SnakeFilter::internalForce_y(int i, vtkPoints* in_points) {
    return  tension   * continuityForce_y(i, in_points) +
            stiffness * curvatureForce_y(i, in_points);
}

double SnakeFilter::continuityForce_x(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int next = getNextPointId(i, N);

    double* pointPrev = in_points->GetPoint(prev);
    double* pointCurr = in_points->GetPoint(i);
    double* pointNext = in_points->GetPoint(next);

    double dx2 = ( pointNext[0] - ( 2.0 * pointCurr[0] ) + pointPrev[0] );
    //TODO no se si toca por 2
    return dx2 ;
}
//TODO change to continuity
double SnakeFilter::continuityForce_y(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int next = getNextPointId(i, N);

    double* pointPrev = in_points->GetPoint(prev);
    double* pointCurr = in_points->GetPoint(i);
    double* pointNext = in_points->GetPoint(next);

    double dy2 = ( pointNext[1] - ( 2.0 * pointCurr[1] ) + pointPrev[1] );
    //TODO no se si toca por 2
    return dy2 ;
}

double SnakeFilter::curvatureForce_x(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int prev2 = getPrevPointId(prev, N);
    int next = getNextPointId(i, N);
    int next2 = getNextPointId(next, N);

    double* pointPrev = in_points->GetPoint(prev);
    double* pointPrev2 = in_points->GetPoint(prev2);
    double* pointCurr = in_points->GetPoint(i);
    double* pointNext = in_points->GetPoint(next);
    double* pointNext2 = in_points->GetPoint(next2);

    double dx4 = ( - pointPrev2[0]
                   + ( 4.0 * ( pointPrev[0] + pointNext[0] ) )
                   - ( 6.0 * pointCurr[0] ) - pointNext2[0] );
    return dx4;
}

double SnakeFilter::curvatureForce_y(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int prev2 = getPrevPointId(prev, N);
    int next = getNextPointId(i, N);
    int next2 = getNextPointId(next, N);

    double* pointPrev = in_points->GetPoint(prev);
    double* pointPrev2 = in_points->GetPoint(prev2);
    double* pointCurr = in_points->GetPoint(i);
    double* pointNext = in_points->GetPoint(next);
    double* pointNext2 = in_points->GetPoint(next2);

    double dy4 = ( - pointPrev2[1]
                   + ( 4.0 * ( pointPrev[1] + pointNext[1] ) )
                   - ( 6.0 * pointCurr[1] ) - pointNext2[1] );
    return  dy4;
}

int SnakeFilter::getPrevPointId(int i, int N) {
    if(i-1 == -1) return N-1;
    else         return i-1;
}

int SnakeFilter::getNextPointId(int i, int N) {
    return (i+1) % N;
}
