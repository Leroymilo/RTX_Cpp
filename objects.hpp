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
        float operator*(const Vect& other);
        Vect operator*(const float& k);
        Vect operator^(const Vect& other);
        Point operator+(const Point& origin);
        Vect operator-(const Vect& other);

        float get_norm();
        Vect normalize();

        friend std::ostream& operator<<(std::ostream&, const Vect&);
};

struct Diff_coef    //Diffusion coefficient for spheres
{
    float r = 1;
    float g = 1;
    float b = 1;
    Diff_coef();
    Diff_coef(float r, float g, float b);
    Diff_coef operator*(const float& k);
};

struct Color
{
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    Color();
    Color(uint8_t r, uint8_t g, uint8_t b);
    Color& operator+=(const Color& other);
    Color operator*(const Diff_coef& Kd);
    Color operator*(const float& Kr);
    bool operator==(const Color& other);
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
        Diff_coef Kd;   //diffusion coefficient
        float Kr;       //reflection coefficient
    
    public :
        Sphere();
        Sphere(Point center, float radius, Diff_coef Kd, float Kr);
        Sphere(Point A, Point B);

        Point get_center();
        float get_radius();
        float get_Kr();
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

        Vect get_dir();
        Point get_point(float dist_from_origin);
        int intersect(Sphere sphere, Point* first_inter, float* dist_from_o);
};

#endif //OBJECTS_H