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

class Triangle
{
    private :
        Point A, B, C;
        Vect n;     //normal vector
        float d;    //distance between the triangle's plane and the origin (0, 0, 0)
        Diff_coef Kd;   //diffusion coefficient
        float Kr;       //reflection coefficient

    public :
        Triangle();
        Triangle(Point A, Point B, Point C, Diff_coef Kd, float Kr);

        Vect get_n();
        float get_d();
        float get_Kr();

        bool is_around(const Point& point);
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
        bool intersect_sph(Sphere* sphere, Intersection*);
        bool intersect_tri(Triangle* triangle, Intersection*);
};

#endif //OBJECTS_H