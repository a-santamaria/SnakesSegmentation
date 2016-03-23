#include <algorithm>
#include <iostream>
#include <cfloat>
#include <cmath>
#include "point.h"

using namespace std;

bool Point::operator< (const Point &other) const{
    return x < other.x;
}

double randF(){
    return ((double)rand() / RAND_MAX);
}

void randEllipse( double r1, double r2, double cx, double cy, double d,
                  double& x, double& y)
{
    double rx = randF()*r1;
    double ry = randF()*r2;
    double theta = randF() * 2 * M_PI;
    double a = sqrt(rx);
    double b = sqrt(ry);
    double alpha = d * M_PI;
    x = cx + ( a * cos(theta) * cos(alpha) ) - ( b * sin(theta) * sin(alpha) );
    y = cy + ( a * cos(theta) * sin(alpha) ) + ( b * sin(theta) * cos(alpha) );
}

double Vec::cross(Vec b){
    return this->x * b.y - this->y * b.x;
}

Vec toVec(Point p, Point q){
    return Vec(q.x - p.x, q.y - p.y);
}

bool ccw(Point p, Point q, Point r){
    return toVec(q, p).cross( toVec(q, r) ) < 0;
}

bool collinear(Point p, Point q, Point r){
    return toVec(q, p).cross( toVec(q, r) ) == 0;
}

double dist(Point p, Point q){
    double x = q.x-p.x;
    double y = q.y-p.y;
    return sqrt(x*x + y*y);
}

bool CompByAngleStruct::operator() (const Point& p, const Point& q){

    if(collinear(piv, p, q)){
        return dist(piv, p) < dist(piv, q);
    }

    return ccw(p, q, piv);
}
