#include "objects.hpp"

#include <cmath>


// Methods for Source struct

Source::Source() {}

Source::Source(Point O, sf::Color col) : O(O), col(col) {}


// Methods for Sphere class

Sphere::Sphere() {}

Sphere::Sphere(Point center, float radius, Diff_coef Kd, float Kr) :
C(center), r(radius), Kd(Kd), Kr(Kr) {}

Point Sphere::get_center()
{
    return C;
}

float Sphere::get_radius()
{
    return r;
}

float Sphere::get_Kr()
{
    return Kr;
}

bool Sphere::above_h(Point P, Source S)
{
    // P is a point of the sphere.
    // Returns true if S is visible from P
    // i.e. if src is above the horizon of P.

    return Vect(C, P) * Vect(P, S.O) > 0;
}

sf::Color Sphere::diffuse(Point P, Source S)
{
    // Returns the color that would result from the diffusion of
    // light source S on the point P of the sphere.

    Vect N = Vect(C, P)*(1/r);
    Vect u = Vect(P, S.O).normalize();
    float cos_th = N*u;

    return S.col*(Kd*cos_th);
}


// Methods for Triangle class

Triangle::Triangle() {}

Triangle::Triangle(Point A, Point B, Point C, Diff_coef Kd, float Kr) : A(A), B(B), C(C), Kd(Kd), Kr(Kr)
{
    n = (Vect(A, B)^Vect(A, C)).normalize();
    d = n*Vect(A, Point(0, 0, 0));
}

Vect Triangle::get_n()
{
    return n;
}

float Triangle::get_d()
{
    return d;
}

float Triangle::get_Kr()
{
    return Kr;
}

// Copied this method from 
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
bool Triangle::is_around(const Point& P)
{
    return (n*(Vect(A, B)^Vect(A, P)) > 0 &&
    n*(Vect(B, C)^Vect(B, P)) > 0 &&
    n*(Vect(C, A)^Vect(C, P)) > 0);
}

sf::Color Triangle::diffuse(Point P, Source S)
{
    // Returns the color that would result from the diffusion of
    // light source S on the point P of the sphere.

    Vect u = Vect(S.O, P).normalize();
    float cos_th = std::abs(n*u);

    return S.col*(Kd*cos_th);
}


// Methods for Ray class

Ray::Ray() {}

Ray::Ray(Point origin, Vect direction) : origin(origin)
{
    if (direction.get_norm() == 1)
        this->direction = direction;
    else
        this->direction = direction.normalize();
}

Ray::Ray(Point A, Point B) : origin(A), direction(Vect(A, B).normalize()) {}

Point Ray::get_point(float d)
{
    return direction * d + origin;
}

Vect Ray::get_dir()
{
    return direction;
}

bool Ray::intersect_sph(Sphere* sphere, Intersection* inter)
{
    // Returns true if the sphere is intersected, else false

    Point C = sphere->get_center();
    float r = sphere->get_radius();
    Vect OC(origin, C);
    // distance of the point of the ray closest to the center of the sphere :
    float t = OC * direction;
    Point P = get_point(t);

    // dist(ray, sphere center) :
    float y = Vect(P, C).get_norm();

    if (y > r)
        return false;   //ray doesn't cross the sphere

    inter->side = OC.get_norm() >= r;
    float dt = std::sqrt(r*r - y*y);
    inter->dist = inter->side ? t - dt : t + dt;

    if (inter->dist < 0)
        return false;   //sphere behind the ray

    inter->point = get_point(inter->dist);
    return true;
}

bool Ray::intersect_tri(Triangle* tri, Intersection* inter)
{
    // Returns true if the triangle is intersected, else false

    Vect N = tri->get_n();
    float d = tri->get_d();

    float den = N*direction;
    if (den == 0)
        return false;   //ray parallel to triangle

    inter->dist = -(d+N*Vect(Point(0, 0, 0), origin)) / den;
    if (inter->dist < 0)
        return false;   //triangle behind ray

    inter->point = get_point(inter->dist);

    if (!tri->is_around(inter->point))
        return false;   //ray doesn't cross the triangle

    inter->side = Vect(origin, inter->point)*N < 0;
    return true;
}