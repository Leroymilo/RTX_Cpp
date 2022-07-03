#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdint.h>
#include <iostream>

struct Point
{
    float x = 0;
    float y = 0;
    float z = 0;

    Point();
    Point(float x, float y, float z);
};

class Vect
{
    private :
        float x = 0;
        float y = 0;
        float z = 0;
    
    public :
        Vect();
        Vect(float x, float y, float z);
        Vect(Point A, Point B);
        float operator*(Vect other);
        Vect operator*(float t);
        Vect operator^(Vect other);
        Point operator+(Point origin);

        float get_norm();
        Vect normalize();
};

struct Color
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color& operator+=(const Color& other);
};

struct Source    //light source
{
    Point S;
    Color C;
    Source();
    Source(Point S, Color C);
};

class Sphere
{
    private :
        Point center;
        float radius;
        Color Kd;   //diffusion coefficient
    
    public :
        Sphere();
        Sphere(Point center, float radius);
        Sphere(Point A, Point B);

        Point get_center();
        float get_radius();
        bool above_h(Point P, Source S);
        Color diffuse(Point P, Source S);
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

        Point get_point(float dist_from_origin);
        int intersect(Sphere sphere, Point* first_inter, float* dist_from_o);
};

#endif //OBJECTS_H