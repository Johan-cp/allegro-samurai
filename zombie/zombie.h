#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <allegro5/allegro.h>
#include <memory>

class Zombie
{
public:
    float x, y;
    float width, height;
    bool isAlive;
    std::shared_ptr<ALLEGRO_BITMAP> image;

    Zombie(float player_width, float player_height, const char *image_route);
    void draw();
    void move(float player_x, float player_y);
    bool collision(float player_x, float player_y, float player_width, float player_height);
};

#endif
