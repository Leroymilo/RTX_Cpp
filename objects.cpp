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

    Vect N = Vect(C, P).normalize();
    Vect u = Vect(P, S.O).normalize();
    float cos_th = N*u;

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

int Ray::intersect_sph(Sphere sphere, Point* first_inter, float* dist)
{
    // returns the number of intersections between the ray and the sphere.

    Point C = sphere.get_center();
    float r = sphere.get_radius();
    Vect OC(origin, C);
    // distance of the point of the ray closest to the center of the sphere :
    float t = OC * direction;
    Point P = get_point(t);

    // dist(ray, sphere center) :
    float y = Vect(P, C).get_norm();

    if (y > r)
        return 0;
    
    if (OC.get_norm() >= r) //General case
    {
        float t1 = t - std::sqrt(r*r - y*y);
        *dist = t1;
        *first_inter = get_point(t1);
        if (y == r)
            return 1;
        return 2;
    }
    else    //When the origin of the ray is inside the sphere
    {
        float t2 = t + std::sqrt(r*r - y*y);
        *dist = t2;
        *first_inter = get_point(t2);
        return 1;
    }

}