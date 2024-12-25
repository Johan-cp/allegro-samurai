// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>

class Player
{
public:
    ALLEGRO_BITMAP *images_idle1;
    ALLEGRO_BITMAP *images_idle2;
    ALLEGRO_BITMAP *images_walk1;
    ALLEGRO_BITMAP *images_walk2;
    ALLEGRO_BITMAP *images_walk3;
    ALLEGRO_BITMAP *images_walk4;
    ALLEGRO_BITMAP *images_punch1[13];
    ALLEGRO_BITMAP *images_punch2[11];
    float x, y;
    float scale_x, scale_y;
    bool in_movement;
    bool facing_left;
    bool puching;
    int punch_animation;
    int actual_frame;
    int punch_frame;

    Player(const char *route_idle1, const char *route_idle2, const char *route_walk1, const char *route_walk2, const char *route_walk3, const char *route_walk4, const char *route_punch1[], const char *route_punch2[], float screen_width, float screen_height, float scale_x, float scale_y);
    ~Player();
    void draw();
    void change_images_idle();
    void change_images_walk();
    void change_images_punch();
    void move(float dx, float dy);
    void punch();
};

#endif
