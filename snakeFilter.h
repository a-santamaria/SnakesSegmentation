#ifndef SNAKE_FILTER_H
#define SNAKE_FILTER_H

#include <vtkPolyDataAlgorithm.h>

#include <vector>
#include "point.h"

class SnakeFilter : public vtkPolyDataAlgorithm {
private:

    double tension = 0.001;
    double stiffness = 0.001;

    double line_weight;
    double edge_weight;
    double term_weight;

    bool fixed;
    bool closed;

    int implicit;

    double atom;
    double tick;

    double threshold;

    vtkDataArray* xGradient;
    vtkDataArray* yGradient;

    std::vector<Point> points;
    std::vector<Point> movedPoints;


public:
    vtkTypeMacro( SnakeFilter,vtkPolyDataAlgorithm );

public:
    static SnakeFilter* New( );

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
    //void update();

private:
    double internalForce_x(int i, vtkPoints* in_points);
    double internalForce_y(int i, vtkPoints* in_points);

    //continuity force
    double continuityForce_x(int i, vtkPoints* in_points);
    double continuityForce_y(int i, vtkPoints* in_points);

    //TODO curvature force
    double curvatureForce_x(int i, vtkPoints* in_points);
    double curvatureForce_y(int i, vtkPoints* in_points);

    int getPrevPointId(int i, int N);
    int getNextPointId(int i, int N);
};

#endif
