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
      std::cout << "points.push_back(Point(" << pos[0] << ", " << pos[1] << "));"<< std::endl;
      points.push_back(Point(pos[0], pos[1]));
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

    virtual void OnRightButtonDown()
    {
      //TODO crear edges y update window
    }

};

vtkStandardNewMacro(customMouseInteractorStyle);

int main(int, char *[])
{

  vtkSmartPointer<vtkPoints> pointsVTK =
        vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkPolyData> pointsPolydata =
     vtkSmartPointer<vtkPolyData>::New();

   pointsPolydata->SetPoints(pointsVTK);

    vtkSmartPointer<vtkPolyData> polyData =
      vtkSmartPointer<vtkPolyData>::New();


    vtkCellArray *cells = vtkCellArray::New();
      cells->Initialize();

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
/*
  std::vector<Point> points;

  points.push_back(Point(42, 408));
points.push_back(Point(39, 299));
points.push_back(Point(113, 364));
points.push_back(Point(127, 127));
points.push_back(Point(276, 329));
points.push_back(Point(358, 91));
points.push_back(Point(443, 563));

/*
  points.push_back(Point(111, 382));
  points.push_back(Point(85, 274));
  points.push_back(Point(241, 227));
  points.push_back(Point(367, 301));
  points.push_back(Point(341, 474));
/*
    points.push_back(Point(120, 434));
    points.push_back(Point(121, 343));
    points.push_back(Point(62, 252));
    points.push_back(Point(168, 157));
    points.push_back(Point(211, 259));
    points.push_back(Point(350, 119));
    points.push_back(Point(452, 339));
    points.push_back(Point(507, 509));
    points.push_back(Point(288, 576));
*/
/*

    points.push_back(Point(93, 371));
    points.push_back(Point(107, 238));
    points.push_back(Point(218, 177));
    points.push_back(Point(261, 280));
    points.push_back(Point(376, 132));
    points.push_back(Point(400, 539));
*/
/*
    points.push_back(Point(87, 429));
    points.push_back(Point(180, 334));
    points.push_back(Point(104, 237));
    points.push_back(Point(224, 247));
    points.push_back(Point(286, 179));
    points.push_back(Point(321, 289));
    points.push_back(Point(430, 128));
    points.push_back(Point(431, 320));
    points.push_back(Point(537, 331));
    points.push_back(Point(480, 417));
    points.push_back(Point(400, 387));
    points.push_back(Point(388, 511));
    points.push_back(Point(318, 467));
    points.push_back(Point(273, 554));
    points.push_back(Point(176, 535));
*/
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

  Snake s(points, 1, 1);
  pointsPolydata->SetPoints(pointsVTK);








  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter =
    vtkSmartPointer<vtkVertexGlyphFilter>::New();

  vertexFilter->SetInputData(pointsPolydata);
  vertexFilter->Update();

   polyData =
    vtkSmartPointer<vtkPolyData>::New();
  polyData->ShallowCopy(vertexFilter->GetOutput());

  polyData->SetLines(cells);

  // Visualization
  mapper->SetInputData(polyData);

  actor =  vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetPointSize(5);

  renderer =  vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(actor);

  renderWindow =  vtkSmartPointer<vtkRenderWindow>::New();



  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize( 600, 600 );

  renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();

    renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
