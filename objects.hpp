#ifndef OBJECTS_H
#define OBJECTS_H

#include "base.hpp"

struct Source    //light source
{
    Point O;    //origin
    sf::Color col;  //color
    Source();
    Source(Point origin, sf::Color color);
};

class Sphere
{
    private :
        Point C;    //center
        float r;    //radius
        Diff_coef Kd;   //diffusion coefficient
        float Kr;       //reflection coefficient
    
    public :
        Sphere();
        Sphere(Point center, float radius, Diff_coef Kd, float Kr);

        Point get_center();
        float get_radius();
        float get_Kr();
        bool above_h(Point P, Source S);
        sf::Color diffuse(Point P, Source S);
};

class Ray
{
    private :
        Point origin;
        Vect direction;
    
    public :
        Ray();
        Ray(Point origin, Vect direction);
        Ray(Point A, Point B);

        Vect get_dir();
        Point get_point(float dist_from_origin);
        int intersect_sph(Sphere sphere, Point* first_inter, float* dist_from_o);
};

#endif //OBJECTS_H