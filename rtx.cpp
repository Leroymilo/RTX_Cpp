#include "objects.hpp"

#include <json/value.h>
#include <json/json.h>
#include <json/writer.h>

#include <list>
#include <thread>
#include <fstream>
#include <cmath>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace filesystem;


// Variables declaration :

int nb_spheres;
vector<Sphere> spheres;
int nb_triangles;
vector<Triangle> triangles; 
int nb_sources;
vector<Source> sources;

unsigned int screen_pxl_w;
unsigned int screen_pxl_h;
float virt_ratio;
float screen_w;
float screen_h;
Point screen_pos;

float rot;
float azi;
float delta_rot;
Point camera_pos;

constexpr double pi() { return atan(1)*4; }

int max_reflect;

sf::Image image;
sf::Color bg_color;

bool multithreading;

// time analyze variables :
sf::Clock sfclock;
float t0;

// files write time variables :
file_time_type settings_t = last_write_time("settings/settings.json");
file_time_type sources_t = last_write_time("settings/sources.json");
file_time_type spheres_t = last_write_time("settings/spheres.json");
file_time_type triangles_t = last_write_time("settings/triangles.json");

// SFML variables :
sf::RenderWindow window;
bool fullscreen;
bool was_fullscreen;


//===========================================================================================================================================//
// Reading jsons :

void read_settings()
{
    //Parsing general settings :
    ifstream file("settings/settings.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse(file, actualJson);
    file.close();

    screen_pxl_w = actualJson["screen_pxl_w"].asInt();
    screen_pxl_h = actualJson["screen_pxl_h"].asInt();

    sf::VideoMode vm = sf::VideoMode::getFullscreenModes()[0];
    if (screen_pxl_w == -1 or screen_pxl_h == -1)
    {
        // I don't want to deal with only one dimension being maxed out
        fullscreen = true;
        screen_pxl_w = vm.width;
        screen_pxl_h = vm.height;
    }
    else fullscreen = false;

    virt_ratio = actualJson["pxl/virt_ratio"].asFloat();
    screen_w = virt_ratio*screen_pxl_w;
    screen_h = virt_ratio*screen_pxl_h;

    float cam_r = actualJson["dist_origin_cam"].asFloat();
    float screen_r = cam_r - actualJson["dist_cam_screen"].asFloat();
    rot = actualJson["cam_rotation"].asFloat()*pi()/180;
    azi = actualJson["cam_azimut"].asFloat()*pi()/180;
    camera_pos = Point(
        cos(azi)*cos(rot)*cam_r,
        cos(azi)*sin(rot)*cam_r,
        sin(azi)*cam_r
    );
    screen_pos = Point(
        cos(azi)*cos(rot)*screen_r,
        cos(azi)*sin(rot)*screen_r,
        sin(azi)*screen_r
    );

    bg_color = sf::Color(
        actualJson["background_color"]["r"].asInt(),
        actualJson["background_color"]["g"].asInt(),
        actualJson["background_color"]["b"].asInt()
    );

    max_reflect = actualJson["max_reflections"].asInt();
    delta_rot = actualJson["delta_rot"].asFloat();

    multithreading = actualJson["multi_threading"].asBool();
}

void resize()
{
    screen_pxl_w = window.getSize().x, screen_pxl_h = window.getSize().y;

    sf::VideoMode vm = sf::VideoMode::getFullscreenModes()[0];
    if (screen_pxl_w == vm.width)   // Not checking height because title bar
    {
        fullscreen = true;
        screen_pxl_h = vm.height;
    }
    else fullscreen = false;
    screen_w = virt_ratio*screen_pxl_w;
    screen_h = virt_ratio*screen_pxl_h;
}

void read_sources()
{
    //Parsing light sources :
    ifstream file("settings/sources.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse(file, actualJson);
    file.close();

    sources.clear();

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
}

void read_spheres()
{
    //Parsing spheres :
    ifstream file("settings/spheres.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse(file, actualJson);
    file.close();

    spheres.clear();

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
}

void read_triangles()
{
    //Parsing triangles :
    ifstream file("settings/triangles.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse(file, actualJson);
    file.close();

    triangles.clear();

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

bool update_settings()
{
    bool changed = false;
    if (settings_t != last_write_time("settings/settings.json"))
    {
        read_settings();
        settings_t = last_write_time("settings/settings.json");
        changed = true;
    }

    if (sources_t != last_write_time("settings/sources.json"))
    {
        read_sources();
        sources_t = last_write_time("settings/sources.json");
        changed = true;
    }

    if (spheres_t != last_write_time("settings/spheres.json"))
    {
        read_spheres();
        spheres_t = last_write_time("settings/spheres.json");
        changed = true;
    }

    if (triangles_t != last_write_time("settings/triangles.json"))
    {
        read_triangles();
        triangles_t = last_write_time("settings/triangles.json");
        changed = true;
    }
    return changed;
}

void rotate(bool dir)
{
    ifstream infile("settings/settings.json");
    Json::Value actualJson;
    Json::Reader reader;
    reader.parse(infile, actualJson);
    infile.close();
    float new_rot;
    if (dir)
        new_rot = fmod(actualJson["cam_rotation"].asInt() + delta_rot, 360);
    else
        new_rot = fmod(actualJson["cam_rotation"].asInt() - delta_rot, 360);
    actualJson["cam_rotation"] = Json::Value(new_rot);
    if (fullscreen)
    {
        actualJson["screen_pxl_h"] = Json::Value(-1);
        actualJson["screen_pxl_w"] = Json::Value(-1);
    }
    else
    {
        actualJson["screen_pxl_h"] = Json::Value(screen_pxl_h);
        actualJson["screen_pxl_w"] = Json::Value(screen_pxl_w);
    }
    ofstream outfile("settings/settings.json");
    outfile << actualJson;
    outfile.close();
}


//===========================================================================================================================================//
// Maths functions :

Point get_screen_point(int pxl_x, int pxl_y)
{
    float screen_x = (screen_w/screen_pxl_w) * (pxl_x+0.5) - screen_w/2;
    float screen_y = (screen_h/screen_pxl_h) * (pxl_y+0.5) - screen_h/2;
    float x = screen_pos.x + sin(rot)*screen_x - sin(azi)*cos(rot)*screen_y;
    float y = screen_pos.y - cos(rot)*screen_x - sin(azi)*sin(rot)*screen_y;
    float z = screen_pos.z + cos(azi)*screen_y;
    // I hate space projections
    return Point(x, y, z);
}

bool is_visible(const Intersection& inter, const Source& S)
{
    Ray ray(inter.point, S.O);
    float dist = Vect(inter.point, S.O).get_norm();
    Intersection temp_inter;

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

        if (ray.intersect_sph(&spheres[i], &temp_inter) && temp_inter.dist < dist)
            return false;
    }

    for (int i = 0; i < nb_triangles; i++)
    {
        if (inter.obj_type == 't' && i == inter.obj_id)
            continue;
        
        if (ray.intersect_tri(&triangles[i], &temp_inter) && temp_inter.dist < dist)
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

void trace_ray(int pxl_x, int pxl_y)
{
    //Creating camera ray :
    Ray ray(camera_pos, get_screen_point(pxl_x, pxl_y));

    //Processing color :
    sf::Color pxl_col = reflect(ray);

    if (pxl_col == bg_color)
        return; //no need to draw bg_color on bg_color

    image.setPixel(screen_pxl_w-pxl_x-1, screen_pxl_h-pxl_y-1, pxl_col);
    //The image need to be reversed since screen coordinates are starting from top left.
    
    return;
}


//===========================================================================================================================================//
// General rendering and drawing procedures :

void draw_pxl_hline(int y)
{
    for (int x = 0; x < screen_pxl_w; x++)
    {
        trace_ray(x, y);
    }    
}

void render()
{
    t0 = sfclock.getElapsedTime().asSeconds();

    cout << "starting render" << endl;

    image.create(screen_pxl_w, screen_pxl_h, bg_color);
    if (multithreading)
    {
        int max_threads = thread::hardware_concurrency();
        list<thread> threads;
        cout << "multithreading on " << max_threads << " threads" << endl;

        for (int y = 0; y < screen_pxl_h; y++)
        {
            if (threads.size() >= max_threads)
            {
                threads.front().join();
                threads.pop_front();
            }
            threads.push_back(thread(draw_pxl_hline, y));
        }

        for (thread & t : threads)
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
    cout << "end of render" << endl;
    cout << "render duration : " << t0 << "s" << endl;
}

void draw()
{
    t0 = sfclock.getElapsedTime().asSeconds();

    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    if (fullscreen && !was_fullscreen)
    {
        window.create(sf::VideoMode::getFullscreenModes()[0], "RTX", sf::Style::Fullscreen);
        was_fullscreen = true;
    }
    else if (!fullscreen)
    {
        window.setSize(sf::Vector2u(screen_pxl_w, screen_pxl_h));
        window.setView(sf::View(sf::FloatRect(0, 0, screen_pxl_w, screen_pxl_h)));
        was_fullscreen = false;
    }
    window.draw(sprite);
    window.display();
    t0 = sfclock.getElapsedTime().asSeconds()- t0;

    cout << "time to draw : " << t0 << "s\n\n";
}


//===========================================================================================================================================//
// Well... main... :

int main()
{
    read_settings();
    read_spheres();
    read_sources();
    read_triangles();

    if (fullscreen)
        window.create(sf::VideoMode::getFullscreenModes()[0], "RTX", sf::Style::Fullscreen);
    else
        window.create(sf::VideoMode(screen_pxl_w, screen_pxl_h), "RTX");
    image.create(screen_pxl_w, screen_pxl_h, bg_color);
    render();
    draw();

    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();
            
            else if (evnt.type == sf::Event::Resized)
            {
                resize();
                render();
                draw();
            }

            else if (evnt.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    if(update_settings())
                    {
                        render();
                        draw();
                    }
                }

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && fullscreen)
                {
                    window.create(sf::VideoMode(screen_pxl_w/2, screen_pxl_h/2), "RTX");
                    resize();
                    render();
                    draw();
                }

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    int count = distance(directory_iterator("renders"), directory_iterator());
                    std::stringstream ss;
                    ss << "renders/render" << std::setfill('0') << std::setw(3) << count << ".png";
                    image.saveToFile(ss.str());
                }

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
                {
                    rotate(true);
                    read_settings();
                    render();
                    draw();
                }

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
                {
                    rotate(false);
                    read_settings();
                    render();
                    draw();
                }
            }
        }
    }
    return 0;
}