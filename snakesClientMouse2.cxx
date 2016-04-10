#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkImageActor.h>
#include <vtkImageActorPointPlacer.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkOutlineSource.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSmartPointer.h>
#include "snakes.h"


#define CANVAS_SIZE 500

// -------------------------------------------------------------------------
struct ActorMiniPipeline
{
  vtkSmartPointer< vtkPolyDataMapper > Mapper;
  vtkSmartPointer< vtkActor > Actor;
  void Configure( vtkPolyData* data )
    {
      this->Mapper = vtkSmartPointer< vtkPolyDataMapper >::New( );
      this->Actor = vtkSmartPointer< vtkActor >::New( );
      this->Mapper->SetInputData( data );
      this->Actor->SetMapper( this->Mapper );
    }
};

// -------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
  // 1. Create a drawing canvas
  vtkSmartPointer< vtkImageData > canvas =
    vtkSmartPointer< vtkImageData >::New( );
  canvas->SetExtent( 0, CANVAS_SIZE, 0, CANVAS_SIZE, 0, 0 );
  canvas->AllocateScalars( VTK_UNSIGNED_CHAR, 4 );
  for( unsigned int i = 0; i < CANVAS_SIZE; ++i )
  {
    for( unsigned int j = 0; j < CANVAS_SIZE; ++j )
    {
      canvas->SetScalarComponentFromFloat( i, j, 0, 0, 100 );
      canvas->SetScalarComponentFromFloat( i, j, 0, 1, 100 );
      canvas->SetScalarComponentFromFloat( i, j, 0, 2, 100 );
      canvas->SetScalarComponentFromFloat( i, j, 0, 3, 128 );

    } // rof

  } // rof
  vtkSmartPointer< vtkImageActor > canvas_actor =
    vtkSmartPointer< vtkImageActor >::New( );
  canvas_actor->GetMapper( )->SetInputData( canvas );
  canvas_actor->SetDisplayExtent( canvas->GetExtent( ) );

  // 2. Prepare an outline
  vtkSmartPointer< vtkOutlineSource > outline =
    vtkSmartPointer< vtkOutlineSource >::New( );
  outline->SetBounds( canvas->GetBounds( ) );
  outline->Update( );
  ActorMiniPipeline outline_actor;
  outline_actor.Configure( outline->GetOutput( ) );
  outline_actor.Actor->GetProperty( )->SetColor( 1, 0, 0 );
  outline_actor.Actor->GetProperty( )->SetLineWidth( 2 );

  // 3. Prepare interaction objects
  vtkSmartPointer< vtkImageActorPointPlacer > placer =
    vtkSmartPointer< vtkImageActorPointPlacer >::New( );
  placer->SetImageActor( canvas_actor );
  vtkSmartPointer< vtkPointHandleRepresentation2D > hnd_rep =
    vtkSmartPointer< vtkPointHandleRepresentation2D >::New( );
  hnd_rep->SetPointPlacer( placer );
  hnd_rep->GetProperty( )->SetColor( 1, 1, 0 );
  vtkSmartPointer< vtkSeedRepresentation > seed_rep =
    vtkSmartPointer< vtkSeedRepresentation >::New( );
  seed_rep->SetHandleRepresentation( hnd_rep );

  // 4. Show data
  vtkSmartPointer< vtkRenderer > ren =
    vtkSmartPointer< vtkRenderer >::New( );
  vtkSmartPointer< vtkRenderWindow > win =
    vtkSmartPointer< vtkRenderWindow >::New( );
  win->AddRenderer( ren );
  vtkSmartPointer< vtkRenderWindowInteractor > iren =
       vtkSmartPointer< vtkRenderWindowInteractor >::New( );
  iren->SetRenderWindow( win );

  // 5. Prepare interactors
  vtkSmartPointer< vtkSeedWidget > seed_wdg =
    vtkSmartPointer< vtkSeedWidget >::New( );
  seed_wdg->SetInteractor( iren );
  seed_wdg->SetRepresentation( seed_rep );

  // 6. Start all and wait for execution
  ren->AddActor( canvas_actor );
  ren->AddActor( outline_actor.Actor );
  iren->Initialize( );
  ren->ResetCamera( );
  ren->Render( );
  seed_wdg->On( );
  iren->Start( );

  // 7. Create input polydata
  vtkSmartPointer< vtkPoints > input_points =
    vtkSmartPointer< vtkPoints >::New( );
  vtkSmartPointer< vtkCellArray > input_verts =
    vtkSmartPointer< vtkCellArray >::New( );
  for( unsigned int i = 0; i < seed_rep->GetNumberOfSeeds( ); ++i )
  {
    double pos[ 3 ];
    seed_rep->GetSeedWorldPosition( i, pos );
    input_points->InsertNextPoint( pos );
    input_verts->InsertNextCell( 1 );
    input_verts->InsertCellPoint( i );

  } // rof

  // 8. Prepare input polydata visualization
  vtkSmartPointer< vtkPolyData > input_data =
    vtkSmartPointer< vtkPolyData >::New( );
  input_data->SetPoints( input_points );
  input_data->SetVerts( input_verts );
  ActorMiniPipeline input_data_actor;
  input_data_actor.Configure( input_data );
  input_data_actor.Actor->GetProperty( )->SetColor( 0, 1, 0 );
  input_data_actor.Actor->GetProperty( )->SetPointSize( 20 );

  // Compute convex hull
  // vtkSmartPointer< ConvexHullFilter > ch =
  //   vtkSmartPointer< ConvexHullFilter >::New( );
  // ch->SetInputData( input_data );
  // ch->Update( );

  // Prepate convex hull diagram visualization
  ActorMiniPipeline ch_actor;
  // ch_actor.Configure( ch->GetOutput( ) );
  ch_actor.Actor->GetProperty( )->SetColor( 0, 0, 1 );
  ch_actor.Actor->GetProperty( )->SetLineWidth( 5 );
  ch_actor.Actor->GetProperty( )->SetPointSize( 10 );

  // Show results
  ren->AddActor( input_data_actor.Actor );
  ren->AddActor( ch_actor.Actor );
  iren->Initialize( );
  ren->ResetCamera( );
  ren->Render( );
  seed_wdg->Off( );
  iren->Start( );

  return( 0 );
}

// eof - main.cxx
