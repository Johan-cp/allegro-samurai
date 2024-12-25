#include "zombie.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

Zombie::Zombie(float screen_width, float screen_height, const char *image_route)
{
    image = std::shared_ptr<ALLEGRO_BITMAP>(al_load_bitmap(image_route), al_destroy_bitmap);
    if (!image)
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la imagen del zombie", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    else
    {
        std::cout << "Imagen del zombie cargada correctamente.\n";
    }
    width = al_get_bitmap_width(image.get());
    height = al_get_bitmap_height(image.get());
    isAlive = true;

    if (rand() % 2 == 0)
    {
        x = 0 - width;
    }
    else
    {
        x = screen_width + width;
    }
    y = rand() % static_cast<int>(screen_height);
    std::cout << "Zombie creado en la posición (" << x << ", " << y << ").\n";
}

void Zombie::draw()
{
    if (isAlive)
    {
        float scale = 2.0;
        al_draw_scaled_bitmap(image.get(), 0, 0, width, height, x, y, width * scale, height * scale, 0);
    }
}

void Zombie::move(float player_x, float player_y)
{
    if (isAlive)
    {
        float dx = player_x - x;
        float dy = player_y - y;
        float distance = sqrt(dx * dx + dy * dy);
        x += (dx / distance) * 1.0;
        y += (dy / distance) * 1.0;
    }
}

bool Zombie::collision(float player_x, float player_y, float player_width, float player_height)
{
    return isAlive && x + width > player_x && x < player_x + player_width &&
           y + height > player_y && y < player_y + player_height;
}
