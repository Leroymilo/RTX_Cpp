#include "objects.hpp"
#include <SFML/Graphics.hpp>

int nb_spheres = 1;
std::vector<Sphere> spheres;
int nb_sources = 3;
std::vector<Source> sources;

float screen_x = 20;
float screen_w = 90;
float screen_h = 50;
unsigned int screen_pxl_w = 1800;
unsigned int screen_pxl_h = 1000;

sf::Image render;
sf::Color bg_color(0, 0, 150);

bool is_visible(int sphere_i, Point P, Source S)
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

void trace_ray(int pxl_y, int pxl_z)
{
    //Searching for a sphere :
    float y = (screen_w/screen_pxl_w) * (pxl_y+0.5) - screen_w/2;
    float z = (screen_h/screen_pxl_h) * (pxl_z+0.5) - screen_h/2;
    Ray ray(Point(-20, 0, 0), Point(screen_x, y, z));

    Point inter;
    Point close_inter;
    float min_dist;
    float dist;
    int nb_inter;
    int min_i = -1;

    for (int i = 0; i < nb_spheres; i++)
    {
        nb_inter = ray.intersect(spheres[i], &inter, &dist);
        if (nb_inter != 0)
        {
            if (min_i == -1 || dist < min_dist)
            {
                min_dist = dist;
                min_i = i;
                close_inter = inter;
            }
        }
    }
    
    if (min_i == -1)
    {
        // render.setPixel(y, z, bg_color);
        return;
    }

    Color pxl_col(0, 0, 0);
    for (int j = 0; j < nb_sources; j++)
    {
        if (is_visible(min_i, close_inter, sources[j]))
        {
            // std::cout << "diffuse color of source" << std::endl;
            pxl_col += spheres[min_i].diffuse(close_inter, sources[j]);
        }
    }

    render.setPixel(pxl_y, pxl_z, sf::Color(pxl_col.r, pxl_col.g, pxl_col.b));
    return;
}

int main()
{
    sf::Clock clock;

    render.create(screen_pxl_w, screen_pxl_h, bg_color);
    sf::RenderWindow window(sf::VideoMode(screen_pxl_w, screen_pxl_h), "");
    window.setSize(sf::Vector2u(screen_pxl_w, screen_pxl_h));

    spheres.push_back(Sphere(Point(20, 0, 0), 5));
    sources.push_back(Source(Point(0, 0, 10), Color(255, 0, 0)));
    sources.push_back(Source(Point(0, 8.66, -5), Color(0, 255, 0)));
    sources.push_back(Source(Point(0, -8.66, -5), Color(0, 0, 255)));

    int t0 = clock.getElapsedTime().asMilliseconds();
    for (int y = 0; y < screen_pxl_w; y++)
    {
        for (int z = 0; z < screen_pxl_h; z++)
        {
            trace_ray(y, z);
        }
    }
    std::cout << "end of render" << std::endl;
    std::cout << "render duration : " << clock.getElapsedTime().asMilliseconds()-t0 << "ms" << std::endl;

    sf::Texture texture;
    texture.loadFromImage(render);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    window.draw(sprite);
    window.display();
    
    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
        }
    }
    return 1;
}