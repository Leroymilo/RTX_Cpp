#include "objects.hpp"

#include <json/value.h>
#include <json/json.h>
#include <fstream>

#include <SFML/Graphics.hpp>

int nb_spheres;
std::vector<Sphere> spheres;
int nb_sources;
std::vector<Source> sources;

float screen_x;
float screen_w;
float screen_h;
unsigned int screen_pxl_w;
unsigned int screen_pxl_h;

Point camera_pos;
sf::Image render;
sf::Color bg_color;

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

    bg_color = sf::Color(
        actualJson["background_color"]["r"].asInt(),
        actualJson["background_color"]["g"].asInt(),
        actualJson["background_color"]["b"].asInt()
    );

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
        float radius = actualJson["spheres"][i]["radius"].asInt();
        Diff_coef color(
            actualJson["spheres"][i]["r"].asFloat(),
            actualJson["spheres"][i]["g"].asFloat(),
            actualJson["spheres"][i]["b"].asFloat()
        );

        spheres.push_back(Sphere(center, radius, color));
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

    read_settings();

    render.create(screen_pxl_w, screen_pxl_h, bg_color);
    sf::RenderWindow window(sf::VideoMode(screen_pxl_w, screen_pxl_h), "");
    window.setSize(sf::Vector2u(screen_pxl_w, screen_pxl_h));

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