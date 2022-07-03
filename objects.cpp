#include "objects.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

// Fast inverse squareroot from https://youtu.be/p8u_k2LIZyo
float inv_sqrt(float x)
{
    long i;
    float x2, y;

    x2 = x*0.5F;
    y = x;
    i = *(long*) &y;
    i = 0x5f3759df - (i>>1);
    y = *(float*) &i;
    y = y*(1.5F-(x2*y*y));

    return y;
}


// Methods for Point class

Point::Point() {}

Point::Point(float x, float y, float z) : x(x), y(y), z(z) {}


// Methods for Vect class

Vect::Vect() {}

Vect::Vect(float x, float y, float z) : x(x), y(y), z(z) {}

Vect::Vect(Point A, Point B) : x(B.x-A.x), y(B.y-A.y), z(B.z-A.z) {}

float Vect::operator*(const Vect& other)
{
    return this->x * other.x + this->y * other.y + this->z * other.z;
}

Vect Vect::operator*(const float& k)
{
    return Vect(this->x*k, this->y*k, this->z*k);
}

Vect Vect::operator^(const Vect& other)
{
    float new_x = this->y * other.z - this->z * other.y;
    float new_y = this->z * other.x - this->x * other.z;
    float new_z = this->x * other.y - this->y * other.x;
    return Vect(new_x, new_y, new_z);
}

Point Vect::operator+(const Point& origin)
{
    return Point(x+origin.x, y+origin.y, z+origin.z);
}

Vect Vect::operator-(const Vect& other)
{
    return Vect(x-other.x, y-other.y, z-other.z);
}

float Vect::get_norm()
{
    return sqrt((*this)*(*this));
}

Vect Vect::normalize()
{
    float inv_norm = inv_sqrt((*this)*(*this));
    return Vect(this->x*inv_norm, this->y*inv_norm, this->z*inv_norm);
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

int Ray::intersect(Sphere sphere, Point* first_inter, float* dist)
{
    // returns the number of intersections between the ray and the sphere.

    Point C = sphere.get_center();
    float r = sphere.get_radius();
    Vect OC(origin, C);
    // distance of the point of the ray closest to the center of the sphere :
    float t = OC * direction;
    Point P = get_point(t);

    // dist(ray, sphere) :
    float y = Vect(P, C).get_norm();

    if (y > r)
        return 0;
    
    if (OC.get_norm() >= r)
    {
        float t1 = t - sqrt(r*r - y*y);
        *dist = t1;
        *first_inter = get_point(t1);
        if (y == r)
            return 1;
        return 2;
    }
    else
    {
        float t2 = t + sqrt(r*r - y*y);
        *dist = t2;
        *first_inter = get_point(t2);
        return 1;
    }

}


// Methods for Color struct

Color::Color() {}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

Color& Color::operator+=(const Color& other)
{
    // cout << "adding color " << other << " to " << *this << std::endl;

    int new_r = r + other.r;
    r = min(255, new_r);
    int new_g = g + other.g;
    g = min(255, new_g);
    int new_b = b + other.b;
    b = min(255, new_b);
    return *this;
}

Color Color::operator*(const Diff_coef& diff_coef)
{
    Color result;
    result.r = uint8_t(float(r)*diff_coef.r);
    result.g = uint8_t(float(g)*diff_coef.g);
    result.b = uint8_t(float(b)*diff_coef.b);
    return result;
}

Color Color::operator*(const float& Kr)
{
    Color result;
    result.r = uint8_t(float(r)*Kr);
    result.g = uint8_t(float(g)*Kr);
    result.b = uint8_t(float(b)*Kr);
    return result;
}

bool Color::operator==(const Color& other)
{
    return (r==other.r && g==other.g && b==other.b);
}

// Methods for Diff_coef struct

Diff_coef::Diff_coef() {}

Diff_coef::Diff_coef(float r, float g, float b)
{
    this->r = max(0.F, min(1.F, r));
    this->g = max(0.F, min(1.F, g));
    this->b = max(0.F, min(1.F, b));
}

Diff_coef Diff_coef::operator*(const float& k)
{
    Diff_coef result;
    result.r = max(0.F, min(1.F, r*k));
    result.g = max(0.F, min(1.F, g*k));
    result.b = max(0.F, min(1.F, b*k));
    return result;
}


// Methods for Source struct

Source::Source() {}

Source::Source(Point S, Color C) : S(S), C(C) {}


// Methods for Sphere class

Sphere::Sphere() {}

Sphere::Sphere(Point center, float radius, Diff_coef Kd, float Kr) :
center(center), radius(radius), Kd(Kd), Kr(Kr) {}

Sphere::Sphere(Point A, Point B) : center(A)
{
    radius = Vect(A, B).get_norm();
}

Point Sphere::get_center()
{
    return center;
}

float Sphere::get_radius()
{
    return radius;
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

    return Vect(center, P) * Vect(P, S.S) > 0;
}

Color Sphere::diffuse(Point P, Source S)
{
    // Returns the color that would result from the diffusion of
    // light source S on the point P of the sphere.

    Vect N = Vect(center, P).normalize();
    Vect u = Vect(P, S.S).normalize();
    float cos_th = N*u;

    return S.C*(Kd*cos_th);
}