#include "snakes.h"

Snake::Snake() {}

Snake::Snake(std::vector<Point> _points, double _tension, double _stiffness) :
                    points(_points), tension(_tension), stiffness(_stiffness) {
    movedPoints.assign(points.size(), Point());
}

Snake::Snake(std::vector<Point> _points, double _tension, double _stiffness,
                double _line_weight, double _edge_weight, double _term_weight) :
                    points(_points), tension(_tension), stiffness(_stiffness),
                    line_weight(_line_weight), edge_weight(_edge_weight),
                    term_weight(_term_weight) {

    movedPoints.assign(points.size(), Point());
}

std::vector<Point> Snake::getPoints() {
    return points;
}

void Snake::update() {
    for(int i = 0; i < points.size(); i++) {
        //TODO add image and external forces
        movedPoints[i].x = points[i].x + internalForce_x(i);
        movedPoints[i].y = points[i].y + internalForce_y(i);
    }
    points = movedPoints;
}

double Snake::internalForce_x(int i) {
    //TODO add curvature
    return tension * continuityForce_x(i) + stiffness * curvatureForce_x(i);
}

double Snake::internalForce_y(int i) {
    //TODO add curvature
    return tension * continuityForce_y(i) + stiffness * curvatureForce_x(i);
}

double Snake::continuityForce_x(int i) {
    int prev = getPrevPointId(i);
    int next = getNextPointId(i);
    double dx2 = ( points[next].x - ( 2.0 * points[i].x ) + points[prev].x );
    //TODO no se si toca por 2
    return dx2 ;
}
//TODO change to continuity
double Snake::continuityForce_y(int i) {
    int prev = getPrevPointId(i);
    int next = getNextPointId(i);
    double dy2 = ( points[next].y - ( 2.0 * points[i].y ) + points[prev].y );
    //TODO no se si toca por 2
    return dy2 ;
}

double Snake::curvatureForce_x(int i) {
    int prev = getPrevPointId(i);
    int prev2 = getPrevPointId(prev);
    int next = getNextPointId(i);
    int next2 = getNextPointId(next);

    double dx4 = ( - points[prev2].x
                   + ( 4.0 * ( points[prev].x + points[next].x ) )
                   - ( 6.0 * points[i].x ) - points[next2].x );
    return dx4;
}

double Snake::curvatureForce_y(int i) {
    int prev = getPrevPointId(i);
    int prev2 = getPrevPointId(prev);
    int next = getNextPointId(i);
    int next2 = getNextPointId(next);

    double dy4 = ( - points[prev2].y
                   + ( 4.0 * ( points[prev].y + points[next].y ) )
                   - ( 6.0 * points[i].y ) - points[next2].y );
    return dy4;
}

int Snake::getPrevPointId(int i) {
    if(i-1 == -1) return points.size()-1;
    else         return i-1;
}

int Snake::getNextPointId(int i) {
    return (i+1) % points.size();
}
