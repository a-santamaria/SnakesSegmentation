#ifndef SNAKES_H
#define SNAKES_H

#include <vector>
#include "point.h"

class Snake {
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

public:
    Snake(double _tension, double _stiffness, double _line_weight,
            double _edge_weight, double _term_weight) :
            tension(_tension), stiffness(_stiffness), line_weight(_line_weight),
            edge_weight(_edge_weight), term_weight(_term_weight) {}
    void update();

private:
    void internal();
};

#endif
