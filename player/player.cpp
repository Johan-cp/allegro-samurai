// player.cpp
#include "player.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

Player::Player(const char *route_idle1, const char *route_idle2, const char *route_walk1, const char *route_walk2, const char *route_walk3, const char *route_walk4, const char *route_punch[], const char *route_punch2[], float screen_width, float screen_height, float scale_x, float scale_y)
{
    images_idle1 = al_load_bitmap(route_idle1);
    images_idle2 = al_load_bitmap(route_idle2);
    images_walk1 = al_load_bitmap(route_walk1);
    images_walk2 = al_load_bitmap(route_walk2);
    images_walk3 = al_load_bitmap(route_walk3);
    images_walk4 = al_load_bitmap(route_walk4);
    for (int i = 0; i < 13; i++)
    {
        images_punch1[i] = al_load_bitmap(route_punch[i]);
    }
    for (int i = 0; i < 11; i++)
    {
        images_punch2[i] = al_load_bitmap(route_punch2[i]);
    }
    if (!images_idle1 || !images_idle2 || !images_walk1 || !images_walk2 || !images_walk3 || !images_walk4 ||
        !images_punch1[0] || !images_punch1[1] || !images_punch1[2] || !images_punch1[3] || !images_punch1[4] ||
        !images_punch1[5] || !images_punch1[6] || !images_punch1[7] || !images_punch1[8] || !images_punch1[9] ||
        !images_punch1[10] || !images_punch1[11] || !images_punch1[12] || !images_punch2[0] || !images_punch2[1] ||
        !images_punch2[2] || !images_punch2[3] || !images_punch2[4] || !images_punch2[5] || !images_punch2[6] ||
        !images_punch2[7] || !images_punch2[8] || !images_punch2[9] || !images_punch2[10])
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudieron cargar las imágenes del jugador", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    this->scale_x = scale_x;
    this->scale_y = scale_y;
    x = screen_width / 2 - (al_get_bitmap_width(images_idle1) * scale_x) / 2;
    y = screen_height / 2 - (al_get_bitmap_height(images_idle1) * scale_y) / 2;
    in_movement = false;
    facing_left = false;
    puching = false;
    punch_animation = 0;
    actual_frame = 0;
    punch_frame = 0;
}

Player::~Player()
{
    al_destroy_bitmap(images_idle1);
    al_destroy_bitmap(images_idle2);
    al_destroy_bitmap(images_walk1);
    al_destroy_bitmap(images_walk2);
    al_destroy_bitmap(images_walk3);
    al_destroy_bitmap(images_walk4);
    for (int i = 0; i < 13; i++)
    {
        al_destroy_bitmap(images_punch1[i]);
    }
    for (int i = 0; i < 11; i++)
    {
        al_destroy_bitmap(images_punch2[i]);
    }
}

void Player::draw()
{
    ALLEGRO_BITMAP *actual_image;

    if (puching)
    {
        if (punch_animation == 0)
        {
            actual_image = images_punch1[punch_frame];
        }
        else
        {
            actual_image = images_punch2[punch_frame];
        }
    }
    else if (in_movement)
    {
        if (actual_frame < 3)
        {
            actual_image = images_walk1;
        }
        else if (actual_frame < 6)
        {
            actual_image = images_walk2;
        }
        else if (actual_frame < 9)
        {
            actual_image = images_walk3;
        }
        else
        {
            actual_image = images_walk4;
        }
    }
    else
    {
        actual_image = (actual_frame % 2 == 0) ? images_idle1 : images_idle2;
    }

    if (facing_left)
    {
        al_draw_scaled_bitmap(actual_image, 0, 0, al_get_bitmap_width(actual_image), al_get_bitmap_height(actual_image), x + al_get_bitmap_width(actual_image) * scale_x, y, -al_get_bitmap_width(actual_image) * scale_x, al_get_bitmap_height(actual_image) * scale_y, 0);
    }
    else
    {
        al_draw_scaled_bitmap(actual_image, 0, 0, al_get_bitmap_width(actual_image), al_get_bitmap_height(actual_image), x, y, al_get_bitmap_width(actual_image) * scale_x, al_get_bitmap_height(actual_image) * scale_y, 0);
    }
}

void Player::change_images_idle()
{
    actual_frame = (actual_frame + 1) % 2;
}

void Player::change_images_walk()
{
    actual_frame = (actual_frame + 1) % 12;
}

void Player::change_images_punch()
{
    if (punch_animation == 0)
    {
        punch_frame = (punch_frame + 1) % 13;
        if (punch_frame == 0)
        {
            puching = false;
            punch_animation = 1;
        }
    }
    else
    {
        punch_frame = (punch_frame + 1) % 11;
        if (punch_frame == 0)
        {
            puching = false;
            punch_animation = 0;
        }
    }
}

void Player::move(float dx, float dy)
{
    if (!puching)
    {
        x += dx;
        y += dy;
        in_movement = (dx != 0 || dy != 0);
        if (dx < 0)
        {
            facing_left = true;
        }
        else if (dx > 0)
        {
            facing_left = false;
        }
    }
}

void Player::punch()
{
    if (!puching)
    {
        puching = true;
        punch_frame = 0;
    }
}
