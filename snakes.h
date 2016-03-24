#ifndef SNAKES_H
#define SNAKES_H

#include <vector>
#include "point.h"

class Snake {
private:

    double tension;
    double stiffness;

    double line_weight;
    double edge_weight;
    double term_weight;

    bool fixed;
    bool closed;

    int implicit;

    double atom;
    double tick;

    double threshold;

    std::vector<Point> points;
    std::vector<Point> movedPoints;
public:
    Snake();
    Snake(std::vector<Point> _points, double _tension, double _stiffness);
    Snake(std::vector<Point> _points, double _tension, double _stiffness,
            double _line_weight, double _edge_weight, double _term_weight);
    std::vector<Point> getPoints();
    void update();

private:
    double internalForce_x(int i);
    double internalForce_y(int i);

    //continuity force
    double elasticForce_x(int i);
    double elasticForce_y(int i);

    //TODO curvature force

    int getPrevPointId(int i);
    int getNextPointId(int i);
};

#endif
