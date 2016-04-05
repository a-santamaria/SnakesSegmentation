#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkPointPicker.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkLine.h>
#include <vtkProperty.h>
#include <vtkCellArray.h>
#include <vector>
#include "point.h"
#include "snakes.h"


// Define interaction style
class customMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    std::vector<Point> points;

    static customMouseInteractorStyle* New();
    vtkTypeMacro(customMouseInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown()
    {
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
      int pos[2];
      this->Interactor->GetEventPosition(pos[0], pos[1]);
      std::cout << "points.push_back(Point(" << pos[0] << ", " << pos[1];
      std::cout << "));"<< std::endl;
      points.push_back(Point(pos[0], pos[1]));
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    virtual void OnRightButtonDown()
    {
        //TODO crear edges y update window
    }

};

void systemPause() {
    char systemPouse;
    std::cout << "Press key to continue ..." << std::endl;
    std::cin >> systemPouse;
}

vtkStandardNewMacro(customMouseInteractorStyle);

int main(int, char *[])
{
    vtkSmartPointer<vtkPoints> pointsVTK =
        vtkSmartPointer<vtkPoints>::New();

    vtkSmartPointer<vtkPolyData> polyData =
      vtkSmartPointer<vtkPolyData>::New();

    vtkCellArray *cells = vtkCellArray::New();
      cells->Initialize();
    polyData->SetPoints(pointsVTK);
    polyData->SetLines(cells);

    vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor> actor =
      vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
    //renderer->SetBackground(1,1,1); // Background color white
    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize( 600, 600 );

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow ( renderWindow );

    vtkSmartPointer<customMouseInteractorStyle> style =
    vtkSmartPointer<customMouseInteractorStyle>::New();
    renderWindowInteractor->SetInteractorStyle( style );

    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    std::vector<vtkIdType> ids;
    std::vector<Point> points = style->points;

    cells->Initialize();
    for(int i = 0; i < points.size(); i++){
        vtkIdType id = pointsVTK->InsertNextPoint ( points[i].x, points[i].y, 0 );
        ids.push_back(id);
    }

    for(int i = 0; i < points.size()-1; i++){
        vtkLine *line = vtkLine::New();
        line->GetPointIds()->SetId(0, ids[i]);
        line->GetPointIds()->SetId(1, ids[i+1]);

        cells->InsertNextCell(line);
    }

    vtkLine *line = vtkLine::New();
    line->GetPointIds()->SetId(0, ids[points.size()-1]);
    line->GetPointIds()->SetId(1, ids[0]);

    cells->InsertNextCell(line);

    std::cout << "acabe de crear edges" << std::endl;

    polyData =
    vtkSmartPointer<vtkPolyData>::New();
    //polyData->ShallowCopy(vertexFilter->GetOutput());
    polyData->SetPoints(pointsVTK);
    polyData->SetLines(cells);

    // Visualization
    mapper->SetInputData(polyData);

    actor =  vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    //actor->GetProperty()->SetPointSize(5);

    renderer =  vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    renderWindow =  vtkSmartPointer<vtkRenderWindow>::New();

    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize( 600, 600 );

    renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor->SetRenderWindow(renderWindow);

    //renderWindow->Render();
    renderWindowInteractor->Initialize();

    Snake s(points, 0.001, 0.001);

    double newCoordinates[3] = { 0.0,  0.0, 0.0 };
    int cont = 0;
    while (cont < 800) {
        //std::cout << "cont " << cont << std::endl;

        s.update();

        for(int i = 0; i <  s.getPoints().size(); i++) {
            newCoordinates[0] = s.getPoints()[i].x;
            newCoordinates[1] = s.getPoints()[i].y;
            polyData->GetPoints()->SetPoint(i, newCoordinates);
            /*
            std::cout << polyData->GetPoint(i)[0] << ", ";
            std::cout << polyData->GetPoint(i)[1]<< std::endl;
            */
        }

        renderWindow->Finalize ();
        renderWindow->Start ();
        renderWindow->Render();
        //renderWindowInteractor->GetRenderWindow()->Render();
        cont++;
    }

    systemPause();
    return EXIT_SUCCESS;
}



/**             para usar despues                    **/
class VisualizationSnake {
private:
    vtkSmartPointer<vtkPoints> pointsVTK;
    vtkSmartPointer<vtkCellArray> cells;
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkCellArray> lineCells;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<customMouseInteractorStyle> mouseStyle;

public:
    void init() {
        pointsVTK = vtkSmartPointer<vtkPoints>::New();
        cells = vtkCellArray::New();
        cells->Initialize();
        polyData = vtkSmartPointer<vtkPolyData>::New();
        polyData->SetPoints(pointsVTK);
        polyData->SetLines(cells);

        mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(polyData);

        actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->AddActor(actor);

        renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);
        renderWindow->SetSize( 600, 600 );

        renderWindowInteractor =
                            vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renderWindowInteractor->SetRenderWindow ( renderWindow );

        mouseStyle =  vtkSmartPointer<customMouseInteractorStyle>::New();
        renderWindowInteractor->SetInteractorStyle( mouseStyle );

        renderWindowInteractor->Initialize();
        renderWindowInteractor->Start();
    }
};
