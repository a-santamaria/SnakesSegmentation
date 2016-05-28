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

#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageRGBToHSV.h>
#include <vtkImageGradient.h>
#include <vtkImageExtractComponents.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCallbackCommand.h>

#include "snakeFilter.h"
#include "snakeObserver.h"

#define CANVAS_SIZE 500

// -------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
    //read image
    vtkSmartPointer<vtkImageData> originalImage =
        vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image =
        vtkSmartPointer<vtkImageData>::New();

    // Verify input arguments
    if ( argc < 2 )
    {
        std::cout << "Usage: " << argv[0]
              << " Filename(.png .jpg)" << std::endl;
              return EXIT_FAILURE;
    }

    // Read the image
    vtkSmartPointer<vtkJPEGReader> readerJPG =
    vtkSmartPointer<vtkJPEGReader>::New();
    vtkSmartPointer<vtkPNGReader> readerPNG =
    vtkSmartPointer<vtkPNGReader>::New();
    int tam = strlen(argv[1]);
    if(argv[1][tam-3] == 'j')
        readerJPG->SetFileName(argv[1]);
    else if (argv[1][tam-3] == 'p')
        readerPNG->SetFileName(argv[1]);
    else {
        std::cerr << "wrong extention most be jpg or png" << std::endl;
        return EXIT_FAILURE;
    }

    // Convert to HSV and extract the Value
    vtkSmartPointer<vtkImageRGBToHSV> hsvFilter =
        vtkSmartPointer<vtkImageRGBToHSV>::New();
    if(argv[1][tam-3] == 'j')
        hsvFilter->SetInputConnection(readerJPG->GetOutputPort());
    else
        hsvFilter->SetInputConnection(readerPNG->GetOutputPort());

    vtkSmartPointer<vtkImageExtractComponents> extractValue =
      vtkSmartPointer<vtkImageExtractComponents>::New();
    extractValue->SetInputConnection(hsvFilter->GetOutputPort());
    extractValue->SetComponents(2);
    extractValue->Update();
    image = extractValue->GetOutput();

    // Compute the gradient of the Value
    vtkSmartPointer<vtkImageGradient> gradientFilter =
        vtkSmartPointer<vtkImageGradient>::New();
    /*const char resultName[] = "resultGrad";
    gradientFilter->SetResultArrayName(resultName);
    */
    gradientFilter->SetInputData(image);
    gradientFilter->SetDimensionality(2);
    gradientFilter->Update();

    vtkImageData* outputGradient = gradientFilter->GetOutput();

    // Extract the x component of the gradient
    vtkSmartPointer<vtkImageExtractComponents> extractXFilter =
        vtkSmartPointer<vtkImageExtractComponents>::New();
    extractXFilter->SetComponents(0);
    extractXFilter->SetInputConnection(gradientFilter->GetOutputPort());
    extractXFilter->Update();
    vtkDataArray* xGradient = extractXFilter->GetOutput()->GetPointData()->GetScalars();

    // Extract the y component of the gradient
    vtkSmartPointer<vtkImageExtractComponents> extractYFilter =
        vtkSmartPointer<vtkImageExtractComponents>::New();
    extractYFilter->SetComponents(1);
    extractYFilter->SetInputConnection(gradientFilter->GetOutputPort());
    extractYFilter->Update();
    vtkDataArray* yGradient = extractYFilter->GetOutput()->GetPointData()->GetScalars();

    // double xRange[2];
    // xGradient->GetRange( xRange );
    // int nT = xGradient->GetNumberOfTuples();
    // std::cout << "range " << xRange[0] << " " << xRange[1] << std::endl;
    // std::cout << "numer of tuples " << nT << std::endl;

    //TODO change to read image like vision
    if(argv[1][tam-3] == 'j') readerJPG->Update();
    else                      readerPNG->Update();

    vtkSmartPointer< vtkImageData > canvas = vtkSmartPointer< vtkImageData >::New();

    if(argv[1][tam-3] == 'j')
        originalImage = readerJPG->GetOutput();
    else
        originalImage = readerPNG->GetOutput();

    int* dims = outputGradient->GetDimensions();
    int extent[6];
    double origin[3];
    double spacing[3];
    originalImage->GetOrigin( origin );
    originalImage->GetSpacing( spacing );
    originalImage->GetExtent( extent );
    std::cout << "dims " << dims[0] << " "<<dims[1] << " "<<dims[2] << std::endl;
    canvas->SetExtent( 0, dims[0], 0, dims[1], 0, 0 );
    canvas->AllocateScalars( VTK_UNSIGNED_CHAR, 4 );
    for( unsigned int i = 0; i < dims[0]; ++i )
    {
        for( unsigned int j = 0; j < dims[1]; ++j )
        {
          int pixel = originalImage->GetScalarComponentAsDouble(i, j, 0, 0);
          canvas->SetScalarComponentFromFloat( i, j, 0, 0, pixel );
          canvas->SetScalarComponentFromFloat( i, j, 0, 1, pixel );
          canvas->SetScalarComponentFromFloat( i, j, 0, 2, pixel );
          canvas->SetScalarComponentFromFloat( i, j, 0, 3, 255 );
          //std::cout << pixel << std::endl;
        } // rof
    } // rof


    vtkSmartPointer< vtkImageActor > canvas_actor =
    vtkSmartPointer< vtkImageActor >::New( );
    canvas_actor->GetMapper( )->SetInputData( canvas );
    canvas_actor->SetDisplayExtent( canvas->GetExtent( ) );

    // Prepare an outline
    vtkSmartPointer< vtkOutlineSource > outline =
    vtkSmartPointer< vtkOutlineSource >::New( );
    outline->SetBounds( canvas->GetBounds( ) );
    outline->Update( );
    ActorMiniPipeline outline_actor;
    outline_actor.Configure( outline->GetOutput( ) );
    outline_actor.Actor->GetProperty( )->SetColor( 1, 0, 0 );
    outline_actor.Actor->GetProperty( )->SetLineWidth( 2 );

    // Prepare interaction objects
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

    // Show data
    vtkSmartPointer< vtkRenderer > ren =
    vtkSmartPointer< vtkRenderer >::New( );
    vtkSmartPointer< vtkRenderWindow > win =
    vtkSmartPointer< vtkRenderWindow >::New( );
    win->AddRenderer( ren );
    vtkSmartPointer< vtkRenderWindowInteractor > iren =
       vtkSmartPointer< vtkRenderWindowInteractor >::New( );
    iren->SetRenderWindow( win );

    // Prepare interactors
    vtkSmartPointer< vtkSeedWidget > seed_wdg =
    vtkSmartPointer< vtkSeedWidget >::New( );
    seed_wdg->SetInteractor( iren );
    seed_wdg->SetRepresentation( seed_rep );

    // Start all and wait for execution
    ren->AddActor( canvas_actor );
    ren->AddActor( outline_actor.Actor );
    iren->Initialize( );
    ren->ResetCamera( );
    ren->Render( );
    seed_wdg->On( );
    iren->Start( );


    // Create input polydata
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

    // Prepare input polydata visualization
    vtkSmartPointer< vtkPolyData > input_data =
    vtkSmartPointer< vtkPolyData >::New( );
    input_data->SetPoints( input_points );
    input_data->SetVerts( input_verts );
    ActorMiniPipeline input_data_actor;
    input_data_actor.Configure( input_data );
    input_data_actor.Actor->GetProperty( )->SetColor( 0, 1, 0 );
    input_data_actor.Actor->GetProperty( )->SetPointSize( 20 );

    // Compute snake
    vtkSmartPointer< SnakeFilter > snake =
    vtkSmartPointer< SnakeFilter >::New( );
    snake->setGradientComponents(xGradient, yGradient);
    snake->setImageSize(dims[0], dims[1]);
    snake->SetInputData( input_data );

    vtkSmartPointer<vtkCallbackCommand> callback =
        vtkSmartPointer<vtkCallbackCommand>::New();
    SnakeObserver* sObserver = new SnakeObserver(canvas_actor);
    callback->SetCallback(sObserver->CallbackFunction);
    snake->AddObserver( snake->RefreshEvent, callback );
    snake->Update( );
    /*
    ActorMiniPipeline snake_actor;
    vtkSmartPointer<vtkPolyData> salida = snake->GetOutput( );
    snake_actor.Configure( salida );
    snake_actor.Actor->GetProperty( )->SetColor( 0, 0, 1 );
    snake_actor.Actor->GetProperty( )->SetLineWidth( 5 );
    snake_actor.Actor->GetProperty( )->SetPointSize( 10 );

    vtkSmartPointer< vtkRenderer > ren2 = vtkSmartPointer< vtkRenderer >::New();
    ren2->AddActor( snake_actor.Actor );
    ren2->AddActor( canvas_actor );
    win->RemoveRenderer( ren );
    win->AddRenderer( ren2 );
    win->Render();
    //iren->SetRenderWindow( win );
    //ren->AddActor( input_data_actor.Actor );
    //ren->AddActor( snake_actor.Actor );
    //iren->Initialize( );
    /*ren->ResetCamera( );
    ren->Render( );
    * /
    int ii = 0;
    while( ii < 800 ) {
        // Compute convex hull

        vtkSmartPointer< SnakeFilter > snake2 =
        vtkSmartPointer< SnakeFilter >::New( );
        snake2->setGradientComponents(xGradient, yGradient);
        snake2->setImageSize(dims[0], dims[1]);
        snake2->SetInputData( salida );
        snake2->Update();

        // Prepate convex hull diagram visualization
        ActorMiniPipeline snake_actor;

        for(int i = 0; i < snake2->GetOutput()->GetNumberOfPoints(); i++){
          double* pos ;
          pos = snake2->GetOutput()->GetPoint(i);
          salida->GetPoints()->SetPoint(i, pos);
        }

        // Show results
        //salida = snake2->GetOutput( );
        /*
        ren->AddActor( input_data_actor.Actor );
        ren->AddActor( snake_actor.Actor );
        iren->Initialize( );
        ren->ResetCamera( );
        ren->Render( );
        seed_wdg->Off( );
        iren->Start( );
        * /
        win->RemoveRenderer( ren2 );
        win->AddRenderer( ren2 );

        win->Render();
        //ren2->Render();
        // seed_wdg->Off( );
        // //ren->Render( );
        // win->Finalize();
        // win->Start();
        ii++;
    }
    */

    return( 0 );
}


// eof - main.cxx
