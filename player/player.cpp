// player.cpp
#include "player.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

Player::Player(const char *ruta_idle1, const char *ruta_idle2, const char *ruta_walk1, const char *ruta_walk2, const char *ruta_walk3, const char *ruta_walk4, const char *rutas_golpe1[], const char *rutas_golpe2[], float pantalla_ancho, float pantalla_alto, float escala_x, float escala_y)
{
    imagen_idle1 = al_load_bitmap(ruta_idle1);
    imagen_idle2 = al_load_bitmap(ruta_idle2);
    imagen_walk1 = al_load_bitmap(ruta_walk1);
    imagen_walk2 = al_load_bitmap(ruta_walk2);
    imagen_walk3 = al_load_bitmap(ruta_walk3);
    imagen_walk4 = al_load_bitmap(ruta_walk4);
    for (int i = 0; i < 13; i++)
    {
        imagen_golpe1[i] = al_load_bitmap(rutas_golpe1[i]);
    }
    for (int i = 0; i < 11; i++)
    {
        imagen_golpe2[i] = al_load_bitmap(rutas_golpe2[i]);
    }
    if (!imagen_idle1 || !imagen_idle2 || !imagen_walk1 || !imagen_walk2 || !imagen_walk3 || !imagen_walk4 ||
        !imagen_golpe1[0] || !imagen_golpe1[1] || !imagen_golpe1[2] || !imagen_golpe1[3] || !imagen_golpe1[4] ||
        !imagen_golpe1[5] || !imagen_golpe1[6] || !imagen_golpe1[7] || !imagen_golpe1[8] || !imagen_golpe1[9] ||
        !imagen_golpe1[10] || !imagen_golpe1[11] || !imagen_golpe1[12] || !imagen_golpe2[0] || !imagen_golpe2[1] ||
        !imagen_golpe2[2] || !imagen_golpe2[3] || !imagen_golpe2[4] || !imagen_golpe2[5] || !imagen_golpe2[6] ||
        !imagen_golpe2[7] || !imagen_golpe2[8] || !imagen_golpe2[9] || !imagen_golpe2[10])
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudieron cargar las imágenes del jugador", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    this->escala_x = escala_x;
    this->escala_y = escala_y;
    x = pantalla_ancho / 2 - (al_get_bitmap_width(imagen_idle1) * escala_x) / 2;
    y = pantalla_alto / 2 - (al_get_bitmap_height(imagen_idle1) * escala_y) / 2;
    en_movimiento = false;
    mirando_izquierda = false;
    golpeando = false;
    animacion_golpe = 0; // Inicialmente la primera animación de golpe
    frame_actual = 0;
    frame_golpe = 0;
}

Player::~Player()
{
    al_destroy_bitmap(imagen_idle1);
    al_destroy_bitmap(imagen_idle2);
    al_destroy_bitmap(imagen_walk1);
    al_destroy_bitmap(imagen_walk2);
    al_destroy_bitmap(imagen_walk3);
    al_destroy_bitmap(imagen_walk4);
    for (int i = 0; i < 13; i++)
    {
        al_destroy_bitmap(imagen_golpe1[i]);
    }
    for (int i = 0; i < 11; i++)
    {
        al_destroy_bitmap(imagen_golpe2[i]);
    }
}

void Player::dibujar()
{
    ALLEGRO_BITMAP *imagen_actual;

    if (golpeando)
    {
        if (animacion_golpe == 0)
        {
            imagen_actual = imagen_golpe1[frame_golpe];
        }
        else
        {
            imagen_actual = imagen_golpe2[frame_golpe];
        }
    }
    else if (en_movimiento)
    {
        if (frame_actual < 3)
        {
            imagen_actual = imagen_walk1;
        }
        else if (frame_actual < 6)
        {
            imagen_actual = imagen_walk2;
        }
        else if (frame_actual < 9)
        {
            imagen_actual = imagen_walk3;
        }
        else
        {
            imagen_actual = imagen_walk4;
        }
    }
    else
    {
        imagen_actual = (frame_actual % 2 == 0) ? imagen_idle1 : imagen_idle2;
    }

    if (mirando_izquierda)
    {
        al_draw_scaled_bitmap(imagen_actual, 0, 0, al_get_bitmap_width(imagen_actual), al_get_bitmap_height(imagen_actual), x + al_get_bitmap_width(imagen_actual) * escala_x, y, -al_get_bitmap_width(imagen_actual) * escala_x, al_get_bitmap_height(imagen_actual) * escala_y, 0);
    }
    else
    {
        al_draw_scaled_bitmap(imagen_actual, 0, 0, al_get_bitmap_width(imagen_actual), al_get_bitmap_height(imagen_actual), x, y, al_get_bitmap_width(imagen_actual) * escala_x, al_get_bitmap_height(imagen_actual) * escala_y, 0);
    }
}

void Player::cambiar_imagen_idle()
{
    frame_actual = (frame_actual + 1) % 2; // Alternar entre 0 y 1
}

void Player::cambiar_imagen_walk()
{
    frame_actual = (frame_actual + 1) % 12; // Alternar entre 0 y 11 para 12 fotogramas de caminar
}

void Player::cambiar_imagen_golpe()
{
    if (animacion_golpe == 0)
    {
        frame_golpe = (frame_golpe + 1) % 13; // Alternar entre 0 y 12 para la primera animación de golpe
        if (frame_golpe == 0)
        {
            golpeando = false;   // Terminar la animación de golpe
            animacion_golpe = 1; // Cambiar a la segunda animación de golpe
        }
    }
    else
    {
        frame_golpe = (frame_golpe + 1) % 11; // Alternar entre 0 y 10 para la segunda animación de golpe
        if (frame_golpe == 0)
        {
            golpeando = false;   // Terminar la animación de golpe
            animacion_golpe = 0; // Cambiar a la primera animación de golpe
        }
    }
}

void Player::mover(float dx, float dy)
{
    if (!golpeando)
    {
        x += dx;
        y += dy;
        en_movimiento = (dx != 0 || dy != 0);
        if (dx < 0)
        {
            mirando_izquierda = true;
        }
        else if (dx > 0)
        {
            mirando_izquierda = false;
        }
    }
}

void Player::golpear()
{
    if (!golpeando)
    {
        golpeando = true;
        frame_golpe = 0; // Iniciar la animación de golpe desde el primer fotograma
    }
}
