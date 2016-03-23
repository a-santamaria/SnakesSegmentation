#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "point.h"
#include "snakes.h"

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkProperty.h>
#include "vtkPolyLine.h"

using namespace std;

#define maxNum 10000


int main( int argc, char* argv[] )
{

    if(argc < 2){
        cerr<< "Usage: " << argv[0];
        cerr<< "num_points";
        cerr<<endl;
        cerr<< "\t num_points: number of random points" << endl;
        return -1;
    }

    int NumP = atoi( argv[1] );
    if(NumP < 2){
        cerr<<"num_ponts most be grater than 1"<<endl;
        return -1;
    }

    srand (time(NULL));
    vector<Point> points;

    vtkSmartPointer<vtkPoints> pointsVTK =
      vtkSmartPointer<vtkPoints>::New();

    unsigned char pointColor[3] = {255, 255, 255};

    vtkSmartPointer<vtkUnsignedCharArray> colors =
      vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName ("Colors");

    double x, y;
    for(int i = 0 ; i < NumP; i++){
        randEllipse(10, 10, 0, 0, 1, x, y);
        points.push_back(Point(x, y));
        pointsVTK->InsertNextPoint (x, y, 0);

        colors->InsertNextTupleValue(pointColor);
    }

    clock_t begin = clock();

    vector<Point> contour;


    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    cout << "time: " << elapsed_secs << endl;






/*** ------------- draw points and contour --------------- **/
    vtkSmartPointer<vtkPolyLine> xPolyLine =
      vtkSmartPointer<vtkPolyLine>::New();
    xPolyLine->GetPointIds()->SetNumberOfIds(contour.size());

    for(int i = 0; i < contour.size(); i++)
    {
        vtkIdType id = pointsVTK->InsertNextPoint(contour[i].x, contour[i].y, 0);
        xPolyLine->GetPointIds()->SetId(i,id);
    }

    vtkSmartPointer<vtkCellArray> cells =
      vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(xPolyLine);

    vtkSmartPointer<vtkPolyData> pointsPolydata =
      vtkSmartPointer<vtkPolyData>::New();

    pointsPolydata->SetPoints(pointsVTK);

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
      vtkSmartPointer<vtkVertexGlyphFilter>::New();

    vertexFilter->SetInputData(pointsPolydata);
    vertexFilter->Update();

    vtkSmartPointer<vtkPolyData> polyData =
      vtkSmartPointer<vtkPolyData>::New();
    polyData->ShallowCopy(vertexFilter->GetOutput());

    //polyData->GetPointData()->SetScalars(colors);
    polyData->SetLines(cells);

    // Visualization
    vtkSmartPointer<vtkPolyDataMapper> mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(5);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize( 600, 600 );

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
