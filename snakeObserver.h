#ifndef SNAKE_OBSERVER_H
#define SNAKE_OBSERVER_H

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include "snakeFilter.h"

struct ActorMiniPipeline
{
    vtkSmartPointer< vtkPolyDataMapper > Mapper;
    vtkSmartPointer< vtkActor > Actor;
    ActorMiniPipeline()
    {
        this->Mapper = vtkSmartPointer< vtkPolyDataMapper >::New( );
        this->Actor = vtkSmartPointer< vtkActor >::New( );
    }
    void Configure( vtkPolyData* data )
    {
        this->Mapper->SetInputData( data );
        this->Actor->SetMapper( this->Mapper );
    }
};

class SnakeObserver
{
private:
    static ActorMiniPipeline snake_actor;
    static vtkSmartPointer< vtkRenderer > ren2;
    static vtkSmartPointer< vtkRenderWindow > win;
public:
    SnakeObserver(vtkSmartPointer< vtkImageActor > canvas_actor);

    static void CallbackFunction(vtkObject* caller,
                    long unsigned int eventId,
                    void* clientData, void* callData );
};

#endif
