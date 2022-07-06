#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <iostream>

#include <SFML/Graphics.hpp>

struct Point
{
    float x = 0, y = 0, z = 0;

    Point();
    Point(float x, float y, float z);
};

class Vect
{
    private :
        float x = 0, y = 0, z = 0;
    
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

class Triangle
{
    private :
        Point A, B, C;
        float a, b, c, d;

    public :

};

struct Diff_coef    //Diffusion coefficient for spheres
{
    float r = 1,  g = 1, b = 1;
    Diff_coef();
    Diff_coef(float r, float g, float b);
    Diff_coef operator*(const float& k);
};

// Operators overloads for sf::Color :
sf::Color operator*(const sf::Color& left, const Diff_coef& diff_coef);

sf::Color operator*(const sf::Color& left, const float& Kr);

std::ostream& operator<<(std::ostream& os, sf::Color& col);

#endif //BASE_H