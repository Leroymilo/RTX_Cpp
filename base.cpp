#include "rtx/base.hpp"

#include <cmath>
#include <algorithm>


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
    return std::sqrt((*this)*(*this));
}

Vect Vect::normalize()
{
    float inv_norm = 1/std::sqrt((*this)*(*this));
    return Vect(this->x*inv_norm, this->y*inv_norm, this->z*inv_norm);
}


// Methods for Diff_coef struct

Diff_coef::Diff_coef() {}

Diff_coef::Diff_coef(float r, float g, float b)
{
    this->r = std::max(0.F, std::min(1.F, r));
    this->g = std::max(0.F, std::min(1.F, g));
    this->b = std::max(0.F, std::min(1.F, b));
}

Diff_coef Diff_coef::operator*(const float& k)
{
    Diff_coef result;
    result.r = std::max(0.F, std::min(1.F, r*k));
    result.g = std::max(0.F, std::min(1.F, g*k));
    result.b = std::max(0.F, std::min(1.F, b*k));
    return result;
}


// Operators overloads for sf::Color :

sf::Color operator*(const sf::Color& left, const Diff_coef& diff_coef)
{
    sf::Color result;
    result.r = uint8_t(float(left.r)*diff_coef.r);
    result.g = uint8_t(float(left.g)*diff_coef.g);
    result.b = uint8_t(float(left.b)*diff_coef.b);
    return result;
}

sf::Color operator*(const sf::Color& left, const float& Kr)
{
    sf::Color result;
    result.r = uint8_t(float(left.r)*Kr);
    result.g = uint8_t(float(left.g)*Kr);
    result.b = uint8_t(float(left.b)*Kr);
    return result;
}

std::ostream& operator<<(std::ostream& os, sf::Color& col)
{
    os << "(" << +col.r;
    os << ", " << +col.g;
    os << ", " << +col.b << ")";
    return os;
}


// Methods for Intersection class

Intersection::Intersection() {}