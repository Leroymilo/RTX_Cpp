#include "objects.hpp"

#include <json/value.h>
#include <json/json.h>

#include <iostream>
#include <list>
#include <thread>
#include <fstream>

#include <SFML/Graphics.hpp>


// operator<< overloads for debug purposes :

// std::ostream& operator<<(std::ostream& os, const Point& p)
// {
//     os << "(" << p.x;
//     os << ", " << p.y;
//     os << ", " << p.z << ")";
//     return os;
// }

// std::ostream& operator<<(std::ostream& os, const Vect& v)
// {
//     os << "(" << v.x;
//     os << ", " << v.y;
//     os << ", " << v.z << ")";
//     return os;
// }

// std::ostream& operator<<(std::ostream& os, Color& col)
// {
//     os << "(" << +col.r;
//     os << ", " << +col.g;
//     os << ", " << +col.b << ")";
//     return os;
// }


// Variables declaration :

int nb_spheres;
std::vector<Sphere> spheres;
int nb_sources;
std::vector<Source> sources;

float screen_x;
float screen_w;
float screen_h;
unsigned int screen_pxl_w;
unsigned int screen_pxl_h;

int max_reflect;

Point camera_pos;
sf::Image render;
Color bg_color;
sf::Color sf_bg_color;

std::list<std::thread> threads;
unsigned long max_threads;

// time analyze variables :
sf::Clock sfclock;


// Reading jsons :

void read_settings()
{
    std::ifstream file("settings/settings.json");
    Json::Value actualJson;
    Json::Reader reader;

    //Parsing general settings :
    reader.parse(file, actualJson);

    screen_x = actualJson["screen_virt_x"].asFloat();
    screen_w = actualJson["screen_virt_w"].asFloat();
    screen_h = actualJson["screen_virt_h"].asFloat();
    screen_pxl_w = actualJson["screen_pxl_w"].asInt();
    screen_pxl_h = actualJson["screen_pxl_h"].asInt();

    camera_pos = Point(
        actualJson["camera_virt_pos"]["x"].asFloat(),
        actualJson["camera_virt_pos"]["y"].asFloat(),
        actualJson["camera_virt_pos"]["z"].asFloat()
    );

    bg_color = Color(
        actualJson["background_color"]["r"].asInt(),
        actualJson["background_color"]["g"].asInt(),
        actualJson["background_color"]["b"].asInt()
    );
    sf_bg_color = sf::Color(bg_color.r, bg_color.g, bg_color.b);

    max_reflect = actualJson["max_reflections"].asInt();

    //Parsing spheres :
    file = std::ifstream("settings/spheres.json");
    reader.parse(file, actualJson);

    nb_spheres = actualJson["nb_spheres"].asInt();

    for (int i = 0; i < nb_spheres; i++)
    {
        Point center(
            actualJson["spheres"][i]["x"].asFloat(),
            actualJson["spheres"][i]["y"].asFloat(),
            actualJson["spheres"][i]["z"].asFloat()
        );
        float radius = actualJson["spheres"][i]["radius"].asFloat();
        Diff_coef color(
            actualJson["spheres"][i]["r"].asFloat(),
            actualJson["spheres"][i]["g"].asFloat(),
            actualJson["spheres"][i]["b"].asFloat()
        );
        float Kr = actualJson["spheres"][i]["Kr"].asFloat();

        spheres.push_back(Sphere(center, radius, color, Kr));
    }

    //Parsing light sources :
    file = std::ifstream("settings/sources.json");
    reader.parse(file, actualJson);

    nb_sources = actualJson["nb_sources"].asInt();

    for (int i = 0; i < nb_sources; i++)
    {
        Point position(
            actualJson["sources"][i]["x"].asFloat(),
            actualJson["sources"][i]["y"].asFloat(),
            actualJson["sources"][i]["z"].asFloat()
        );
        Color color(
            actualJson["sources"][i]["r"].asInt(),
            actualJson["sources"][i]["g"].asInt(),
            actualJson["sources"][i]["b"].asInt()
        );

        sources.push_back(Source(position, color));
    }
}


//Maths functions :

bool is_visible(int sphere_i, Point& P, Source& S)
{
    Sphere* sphere = &spheres[sphere_i];

    if (!sphere->above_h(P, S))
    {
        // std::cout << "source under point's horizon" << std::endl;
        return false;
    }

    Ray ray(P, S.S);
    Point p;
    float d;

    for (int i = 0; i < nb_spheres; i++)
    {
        if (i == sphere_i)
            continue;
        
        if (ray.intersect(spheres[i], &p, &d) && d > 0)
        {
            // std::cout << "source obstructed by a sphere" << std::endl;
            return false;
        }
    }

    // std::cout << "source visible" << std::endl;
    return true;
}

int intersect(Ray& ray, Point* intersection, int prev_sph_i = -1)
{
    //Returns the index of the closest intersected sphere
    Point inter;
    Point close_inter;
    float min_dist;
    float dist;
    int nb_inter;
    int min_i = -1;

    for (int i = 0; i < nb_spheres; i++)
    {
        if (i == prev_sph_i)
            continue;
        
        nb_inter = ray.intersect(spheres[i], &inter, &dist);
        if (nb_inter == 0)
            continue;
        
        if ((min_i == -1 || dist < min_dist) && dist > 0)
        {
            min_dist = dist;
            min_i = i;
            close_inter = inter;
        }
    }

    if (min_i != -1)
        *intersection = close_inter;
    return min_i;
}

Color diffuse(int sphere_i, Point& P)
{
    Color pxl_col(0, 0, 0);

    for (int j = 0; j < nb_sources; j++)
    {
        if (is_visible(sphere_i, P, sources[j]))
            pxl_col += spheres[sphere_i].diffuse(P, sources[j]);
    }

    return pxl_col;
}

Color reflect(Ray& ray, int reflect_no = 0, int prev_sph_i = -1)
{
    //Searching the first sphere intersected by the ray :
    Point P;
    int sphere_i = intersect(ray, &P, prev_sph_i);

    //No sphere intersected :
    if (sphere_i == -1)
    {
        if (reflect_no > 0) //one type of end case : reflection goes to bg
            return Color(0, 0, 0);
        else    //camera ray goes straight to background
            return bg_color;
    }

    //Diffusing the color of the intersected sphere :
    Color color = diffuse(sphere_i, P);
    
    if (reflect_no == max_reflect || spheres[sphere_i].get_Kr() == 0)
    //the other end case : max reflection reached or the sphere is not reflective
        return color;

    //Computing the reflected ray :
    Vect N = Vect(spheres[sphere_i].get_center(), P);
    Vect plan_dir = (N ^ (N ^ ray.get_dir()) ).normalize();
    Vect w = plan_dir * (ray.get_dir() * plan_dir * 2) - ray.get_dir();
    Ray reflected(P, w);

    Color ref_col = reflect(reflected, reflect_no+1, sphere_i);

    color +=  ref_col * spheres[sphere_i].get_Kr();
    return color;
}

void trace_ray(int pxl_y, int pxl_z)
{
    //Creating camera ray :
    float y = (screen_w/screen_pxl_w) * (pxl_y+0.5) - screen_w/2;
    float z = (screen_h/screen_pxl_h) * (pxl_z+0.5) - screen_h/2;
    Ray ray(Point(-20, 0, 0), Point(screen_x, y, z));

    //Processing color :
    Color pxl_col = reflect(ray);
    
    if (pxl_col == bg_color)
        return; //no need to draw bg_color on bg_color

    render.setPixel(pxl_y, screen_pxl_h-pxl_z, sf::Color(pxl_col.r, pxl_col.g, pxl_col.b));
    //The image need to be reversed somehow...
    return;
}

void draw_pxl_hline(int z)
{
    for (int y = 0; y < screen_pxl_w; y++)
        trace_ray(y, z);
}


// Well... main... :

int main()
{
    read_settings();

    render.create(screen_pxl_w, screen_pxl_h, sf_bg_color);

    float t0 = sfclock.getElapsedTime().asSeconds();
    max_threads = std::thread::hardware_concurrency();

    std::cout << "starting render" << std::endl;
    for (int z = 0; z < screen_pxl_h; z++)
    {
        if (threads.size() >= max_threads)
        {
            threads.front().join();
            threads.pop_front();
        }
        threads.push_back(std::thread(draw_pxl_hline, z));
        // draw_pxl_hline(z);
    }

    for (std::thread & t : threads)
    {
        t.join();
    }

    t0 = sfclock.getElapsedTime().asSeconds() - t0;
    std::cout << "end of render" << std::endl;
    std::cout << "render duration : " << t0 << "s" << std::endl;

    sf::RenderWindow window(sf::VideoMode(screen_pxl_w, screen_pxl_h), "");
    window.setSize(sf::Vector2u(screen_pxl_w, screen_pxl_h));
    sf::Texture texture;
    sf::Sprite sprite;

    t0 = sfclock.getElapsedTime().asSeconds();

    texture.loadFromImage(render);

    sprite.setTexture(texture);

    window.draw(sprite);
    window.display();
    t0 = sfclock.getElapsedTime().asSeconds()- t0;

    std::cout << "time to draw : " << t0 << "s" << std::endl;

    
    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
        }
    }
    return 0;
}