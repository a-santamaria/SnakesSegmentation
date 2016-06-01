#ifndef SNAKE_FILTER_H
#define SNAKE_FILTER_H

#include <vtkPolyDataAlgorithm.h>

#include <vector>
#include <cmath>
#include <cassert>
#include "point.h"

class SnakeFilter : public vtkPolyDataAlgorithm {
private:

    double tension = 0.05;
    double stiffness = 0.1;

    double line_weight = 8.0;
    double edge_weight;
    double term_weight;

    double hiGradx;
    double hiGrady;

    double threshold;

    vtkDataArray* xGradient;
    vtkDataArray* yGradient;

    std::vector<Point> points;
    std::vector<Point> movedPoints;

    int imageHeight;
    int imageWidth;


public:
    vtkTypeMacro( SnakeFilter,vtkPolyDataAlgorithm );
    static SnakeFilter* New( );

    int RefreshEvent;

protected:
    SnakeFilter( );
    virtual ~SnakeFilter( );

    // Usual data generation method
    virtual int RequestData(
        vtkInformation* request,
        vtkInformationVector** input,
        vtkInformationVector* output
    );

private:
    // Purposely not implemented.
    SnakeFilter( const SnakeFilter& other );
    void operator=( const SnakeFilter& other );

/**************************************************************/
public:
    //SnakeFilter();
    SnakeFilter(std::vector<Point> _points, double _tension, double _stiffness);
    SnakeFilter(std::vector<Point> _points, double _tension, double _stiffness,
            double _line_weight, double _edge_weight, double _term_weight);
    std::vector<Point> getPoints();
    void setGradientComponents(vtkDataArray* x, vtkDataArray* y);
    void setImageSize(int height, int width);

private:
    double internalForce_x(int i, vtkPoints* in_points);
    double internalForce_y(int i, vtkPoints* in_points);

    //continuity force
    double continuityForce_x(int i, vtkPoints* in_points);
    double continuityForce_y(int i, vtkPoints* in_points);

    // curvature force
    double curvatureForce_x(int i, vtkPoints* in_points);
    double curvatureForce_y(int i, vtkPoints* in_points);

    int getPrevPointId(int i, int N);
    int getNextPointId(int i, int N);

    // image forces
    double imageForce_x(int i, vtkPoints* in_points);
    double imageForce_y(int i, vtkPoints* in_points);

    // gradient
    double getImageGradient_x(int i, vtkPoints* in_points);
    double getImageGradient_y(int i, vtkPoints* in_points);

    int getIdImageAt(int x, int y);
};

#endif
