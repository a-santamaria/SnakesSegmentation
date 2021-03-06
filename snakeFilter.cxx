#include "snakeFilter.h"

#include <vtkCellArray.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>

// -------------------------------------------------------------------------
SnakeFilter* SnakeFilter::
New( ) {
  return( new SnakeFilter( ) );
}

// -------------------------------------------------------------------------
SnakeFilter::
SnakeFilter( ) : vtkPolyDataAlgorithm( )
{
    this->RefreshEvent = vtkCommand::UserEvent - 1;
    threshold = 1e-1;
}

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
    vtkSmartPointer<vtkPoints> in_points = in->GetPoints( );

    // Get output
    vtkInformation* output_info = output->GetInformationObject( 0 );
    vtkSmartPointer<vtkPolyData> out = vtkPolyData::SafeDownCast(
        output_info->Get( vtkDataObject::DATA_OBJECT( ) )
        );

    // Real output objects
    vtkSmartPointer<vtkPoints> out_points;
    vtkSmartPointer<vtkCellArray> out_lines;
    vtkSmartPointer<vtkCellArray> out_verts;

    double currP[3];
    currP[2] = 0;
    int N = in_points->GetNumberOfPoints( );
    int ii = 0;
    //TODO cambiar para que sea con promedio de movimiento
    double avgMovement = 10000;
    double auxX, auxY;
    while( avgMovement > threshold ) {
        avgMovement = 0.0;
        out_points = vtkPoints::New( );
        out_lines = vtkCellArray::New( );
        out_verts = vtkCellArray::New( );

        for(unsigned long i = 0; i < N; i++) {
            currP[0] = in_points->GetPoint(i)[0];
            currP[1] = in_points->GetPoint(i)[1];

            auxX = internalForce_x(i, in_points) + imageForce_x(i, in_points);
            auxY = internalForce_y(i, in_points) + imageForce_y(i, in_points);

            avgMovement += sqrt(auxX*auxX + auxY*auxY);
            currP[0] += auxX;
            currP[1] += auxY;
            out_points->InsertNextPoint(currP);
        }
        avgMovement /= (double)N;

        std::cout << "avg " << avgMovement << " thre " << threshold << std::endl;
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
        this->InvokeEvent(this->RefreshEvent, NULL);
        in_points = out_points;
        ii++;
    }

    std::cout << "----end---" << std::endl;
    return 1;
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

    double pointPrev = in_points->GetPoint(prev)[0];
    double pointCurr = in_points->GetPoint(i)[0];
    double pointNext = in_points->GetPoint(next)[0];

    double dx2 = ( pointNext - ( 2.0 * pointCurr ) + pointPrev );
    //TODO no se si toca por 2
    return dx2 ;
}
//TODO change to continuity
double SnakeFilter::continuityForce_y(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int next = getNextPointId(i, N);

    double pointPrev = in_points->GetPoint(prev)[1];
    double pointCurr = in_points->GetPoint(i)[1];
    double pointNext = in_points->GetPoint(next)[1];

    double dy2 = ( pointNext - ( 2.0 * pointCurr ) + pointPrev );
    //TODO no se si toca por 2
    return dy2 ;
}

double SnakeFilter::curvatureForce_x(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int prev2 = getPrevPointId(prev, N);
    int next = getNextPointId(i, N);
    int next2 = getNextPointId(next, N);

    double pointPrev = in_points->GetPoint(prev)[0];
    double pointPrev2 = in_points->GetPoint(prev2)[0];
    double pointCurr = in_points->GetPoint(i)[0];
    double pointNext = in_points->GetPoint(next)[0];
    double pointNext2 = in_points->GetPoint(next2)[0];

    double dx4 = ( - pointPrev2
                   + ( 4.0 * ( pointPrev + pointNext ) )
                   - ( 6.0 * pointCurr ) - pointNext2 );
    return dx4;
}

double SnakeFilter::curvatureForce_y(int i, vtkPoints* in_points) {
    int N = in_points->GetNumberOfPoints( );
    int prev = getPrevPointId(i, N);
    int prev2 = getPrevPointId(prev, N);
    int next = getNextPointId(i, N);
    int next2 = getNextPointId(next, N);

    double pointPrev = in_points->GetPoint(prev)[1];
    double pointPrev2 = in_points->GetPoint(prev2)[1];
    double pointCurr = in_points->GetPoint(i)[1];
    double pointNext = in_points->GetPoint(next)[1];
    double pointNext2 = in_points->GetPoint(next2)[1];

    double dy4 = ( - pointPrev2
                   + ( 4.0 * ( pointPrev + pointNext ) )
                   - ( 6.0 * pointCurr ) - pointNext2 );
    return  dy4;
}

void SnakeFilter::setGradientComponents(vtkDataArray* x, vtkDataArray* y) {
    xGradient = x;
    yGradient = y;
    hiGradx = 0;
    hiGrady = 0;
    double currx, curry;
    for(int id = 0; id < imageHeight*imageWidth; id++){
        currx = fabs(xGradient->GetTuple1(id));
        if(currx> hiGradx)
            hiGradx = currx;

        curry = fabs(yGradient->GetTuple1(id));
        if(curry > hiGrady)
            hiGrady = curry;
    }
    std::cout << "highest " << hiGradx << " " << hiGrady << std::endl;
}

void SnakeFilter::setImageSize(int height, int width) {
    imageHeight = height;
    imageWidth = width;
}

int SnakeFilter::getPrevPointId(int i, int N) {
    if(i-1 == -1) return N-1;
    else         return i-1;
}

int SnakeFilter::getNextPointId(int i, int N) {
    return (i+1) % N;
}

double SnakeFilter::imageForce_x(int i, vtkPoints* in_points) {
    double gradx = getImageGradient_x(i, in_points);
    // std::cout << "en x " << gradx << " " << hiGradx << ": " << ( gradx / hiGradx) << std::endl;
    return line_weight * ( gradx / hiGradx);
}

double SnakeFilter::imageForce_y(int i, vtkPoints* in_points) {
    double grady = getImageGradient_y(i, in_points);
    // std::cout << "en y " << ( grady / hiGrady) << std::endl;
    return line_weight * ( grady / hiGrady);
}

double SnakeFilter::getImageGradient_x(int i, vtkPoints* in_points) {
    double x = in_points->GetPoint(i)[0];
    double y = in_points->GetPoint(i)[1];
    int id = getIdImageAt( round(x), round(y) );
    if(id >= imageHeight*imageWidth || i < 0) return 0;
    return xGradient->GetTuple1(id);
}

double SnakeFilter::getImageGradient_y(int i, vtkPoints* in_points) {
    double x = in_points->GetPoint(i)[0];
    double y = in_points->GetPoint(i)[1];
    int id = getIdImageAt( round(x), round(y) );
    if(id >= imageHeight*imageWidth || i < 0) return 0;
    return yGradient->GetTuple1(id);
}

int SnakeFilter::getIdImageAt(int x, int y) {
    //TODO revisar que no se pase
    return (y * imageWidth) + x;
}
