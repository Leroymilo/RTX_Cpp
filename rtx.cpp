#include "objects.hpp"

#include <json/value.h>
#include <json/json.h>

#include <list>
#include <thread>
#include <fstream>

// Variables declaration :

int nb_spheres;
std::vector<Sphere> spheres;
int nb_triangles;
std::vector<Triangle> triangles; 
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
sf::Color bg_color;

bool multithreading;

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

    bg_color = sf::Color(
        actualJson["background_color"]["r"].asInt(),
        actualJson["background_color"]["g"].asInt(),
        actualJson["background_color"]["b"].asInt()
    );

    max_reflect = actualJson["max_reflections"].asInt();

    multithreading = actualJson["multi_threading"].asBool();

    //Parsing spheres :
    file = std::ifstream("settings/spheres.json");
    reader.parse(file, actualJson);

    nb_spheres = actualJson["nb_spheres"].asInt();

    for (int i = 0; i < nb_spheres; i++)
    {
        spheres.push_back(Sphere(
            Point(actualJson["spheres"][i]["x"].asFloat(),
                  actualJson["spheres"][i]["y"].asFloat(),
                  actualJson["spheres"][i]["z"].asFloat()),
            actualJson["spheres"][i]["radius"].asFloat(),
            Diff_coef(actualJson["spheres"][i]["r"].asFloat(),
                      actualJson["spheres"][i]["g"].asFloat(),
                      actualJson["spheres"][i]["b"].asFloat()),
            actualJson["spheres"][i]["Kr"].asFloat()
        ));
    }

    //Parsing light sources :
    file = std::ifstream("settings/sources.json");
    reader.parse(file, actualJson);

    nb_sources = actualJson["nb_sources"].asInt();

    for (int i = 0; i < nb_sources; i++)
    {
        sources.push_back(Source(
            Point(actualJson["sources"][i]["x"].asFloat(),
                  actualJson["sources"][i]["y"].asFloat(),
                  actualJson["sources"][i]["z"].asFloat()),
            sf::Color(actualJson["sources"][i]["r"].asInt(),
                      actualJson["sources"][i]["g"].asInt(),
                      actualJson["sources"][i]["b"].asInt())
        ));
    }

    //Parsing triangles :
    file = std::ifstream("settings/triangles.json");
    reader.parse(file, actualJson);

    nb_triangles = actualJson["nb_triangles"].asInt();

    for (int i = 0; i < nb_triangles; i++)
    {
        triangles.push_back(Triangle(
            Point(actualJson["triangles"][i]["A"]["x"].asFloat(),
                  actualJson["triangles"][i]["A"]["y"].asFloat(),
                  actualJson["triangles"][i]["A"]["z"].asFloat()),
            Point(actualJson["triangles"][i]["B"]["x"].asFloat(),
                  actualJson["triangles"][i]["B"]["y"].asFloat(),
                  actualJson["triangles"][i]["B"]["z"].asFloat()),
            Point(actualJson["triangles"][i]["C"]["x"].asFloat(),
                  actualJson["triangles"][i]["C"]["y"].asFloat(),
                  actualJson["triangles"][i]["C"]["z"].asFloat()),
            Diff_coef(actualJson["triangles"][i]["r"].asFloat(),
                      actualJson["triangles"][i]["g"].asFloat(),
                      actualJson["triangles"][i]["b"].asFloat()),
            actualJson["triangles"][i]["Kr"].asFloat()
        ));
    }
}


//Maths functions :

bool is_visible(const Intersection& inter, const Source& S)
{
    Ray ray(inter.point, S.O);
    Intersection _;    //dummy to fill unused parameter

    if (inter.obj_type == 's')
    {
        if (inter.side && !spheres[inter.obj_id].above_h(inter.point, S))
            return false;
    }
    else
    {
        if (inter.side != (ray.get_dir()*triangles[inter.obj_id].get_n() > 0))
            return false;
    }

    for (int i = 0; i < nb_spheres; i++)
    {
        if (inter.obj_type == 's' && i == inter.obj_id)
            continue;

        if (ray.intersect_sph(&spheres[i], &_))
            return false;
    }

    for (int i = 0; i < nb_triangles; i++)
    {
        if (inter.obj_type == 't' && i == inter.obj_id)
            continue;
        
        if (ray.intersect_tri(&triangles[i], &_))
            return false;
    }

    return true;
}

bool intersect_all(Ray& ray, Intersection* inter, const Intersection& prev_inter)
{
    // Returns true if an object is intersected.
    // The data of the intersected object is passed through inter
    Intersection temp_inter;

    for (int i = 0; i < nb_spheres; i++)
    {
        if (prev_inter.obj_type == 's' && prev_inter.obj_id == i)
            continue;
        
        if (!ray.intersect_sph(&spheres[i], &temp_inter))
            continue;
        
        if (inter->obj_id == -1 || temp_inter.dist < inter->dist)
        {
            *inter = temp_inter;
            inter->obj_id = i;
            inter->obj_type = 's';
        }
    }

    for (int i = 0; i < nb_triangles; i++)
    {
        if (prev_inter.obj_type == 't' && prev_inter.obj_id == i)
            continue;
        
        if (!ray.intersect_tri(&triangles[i], &temp_inter))
            continue;
        
        if (inter->obj_id == -1 || temp_inter.dist < inter->dist)
        {
            *inter = temp_inter;
            inter->obj_id = i;
            inter->obj_type = 't';
        }
    }

    return inter->obj_id != -1;
}

sf::Color diffuse(const Intersection& inter)
{
    sf::Color pxl_col(0, 0, 0);

    for (int j = 0; j < nb_sources; j++)
    {
        if (is_visible(inter, sources[j]))
        {
            if (inter.obj_type == 's')
                pxl_col += spheres[inter.obj_id].diffuse(inter.point, sources[j]);
            else
                pxl_col += triangles[inter.obj_id].diffuse(inter.point, sources[j]);
        }
    }

    return pxl_col;
}

sf::Color reflect(Ray& ray, const Intersection &prev_inter = Intersection(), int reflect_no = 0)
{
    //Searching the first sphere intersected by the ray :
    Intersection inter;

    if (!intersect_all(ray, &inter, prev_inter))
    {
        if (reflect_no > 0) //one type of end case : reflection goes to bg
            return sf::Color(0, 0, 0);
        else    //camera ray goes straight to background
            return bg_color;
    }

    // Diffusing the color of the intersected object :
    sf::Color color = diffuse(inter);

    if (reflect_no == max_reflect)  // Max reflection reached
        return color;
    
    // If the object is not reflective :
    if (inter.obj_type == 's')
    {
        if (spheres[inter.obj_id].get_Kr() == 0)
            return color;
    }
    else
    {
        if (triangles[inter.obj_id].get_Kr() == 0)
            return color;
    }

    //Computing the reflected ray :
    Vect N;
    if (inter.obj_type == 's')
        N = Vect(spheres[inter.obj_id].get_center(), inter.point)*(1/spheres[inter.obj_id].get_radius());
    else
        N = triangles[inter.obj_id].get_n();
    Vect w = ray.get_dir() - N * (ray.get_dir() * N * 2);
    Ray reflected(inter.point, w);

    // Computing reflected color :
    if (inter.obj_type == 's')
        color +=  reflect(reflected, inter, reflect_no+1) * spheres[inter.obj_id].get_Kr();
    else
        color +=  reflect(reflected, inter, reflect_no+1) * triangles[inter.obj_id].get_Kr();
        
    return color;
}

void trace_ray(int pxl_y, int pxl_z)
{
    //Creating camera ray :
    float y = (screen_w/screen_pxl_w) * (pxl_y+0.5) - screen_w/2;
    float z = (screen_h/screen_pxl_h) * (pxl_z+0.5) - screen_h/2;
    Ray ray(camera_pos, Point(screen_x, y, z));

    //Processing color :
    sf::Color pxl_col = reflect(ray);

    if (pxl_col == bg_color)
        return; //no need to draw bg_color on bg_color

    render.setPixel(screen_pxl_w-pxl_y-1, screen_pxl_h-pxl_z-1, pxl_col);
    //The image need to be reversed since screen coordinates are starting from top left.
    
    return;
}

void draw_pxl_hline(int z)
{
    for (int y = 0; y < screen_pxl_w; y++)
    {
        trace_ray(y, z);
    }    
}


//===========================================================================================================================================//
// Well... main... :

int main()
{
    read_settings();

    render.create(screen_pxl_w, screen_pxl_h, bg_color);

    float t0 = sfclock.getElapsedTime().asSeconds();

    std::cout << "starting render" << std::endl;
    if (multithreading)
    {
        std::cout << "multithreading" << std::endl;
        int max_threads = std::thread::hardware_concurrency();
        std::list<std::thread> threads;

        for (int z = 0; z < screen_pxl_h; z++)
        {
            if (threads.size() >= max_threads)
            {
                threads.front().join();
                threads.pop_front();
            }
            threads.push_back(std::thread(draw_pxl_hline, z));
        }

        for (std::thread & t : threads)
        {
            t.join();
        }
    }
    else
    {
        for (int z = 0; z < screen_pxl_h; z++)
        {
            draw_pxl_hline(z);
        }
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