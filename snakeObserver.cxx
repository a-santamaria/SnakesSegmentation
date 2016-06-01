#include "snakeObserver.h"

ActorMiniPipeline SnakeObserver::snake_actor;
vtkSmartPointer< vtkRenderer > SnakeObserver::ren2;
vtkSmartPointer< vtkRenderWindow > SnakeObserver::win;


SnakeObserver::SnakeObserver(vtkSmartPointer< vtkImageActor > canvas_actor)
{
    ren2 = vtkSmartPointer< vtkRenderer >::New();
    win = vtkSmartPointer< vtkRenderWindow >::New( );
    snake_actor.Actor->GetProperty( )->SetColor( 0, 0, 1 );
    snake_actor.Actor->GetProperty( )->SetLineWidth( 5 );
    snake_actor.Actor->GetProperty( )->SetPointSize( 10 );
    ren2->AddActor( snake_actor.Actor );
    ren2->AddActor( canvas_actor );
    win->AddRenderer( ren2 );
}

void SnakeObserver::CallbackFunction(vtkObject* caller,
                long unsigned int eventId,
                void* clientData, void* callData )
{
    SnakeFilter* snake = (SnakeFilter*)caller;

    vtkSmartPointer<vtkPolyData> salida = snake->GetOutput( );
    snake_actor.Configure( salida );

    win->Render();
}
