// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <allegro5/allegro.h>

class Player {
public:
    ALLEGRO_BITMAP* imagen_idle1;
    ALLEGRO_BITMAP* imagen_idle2;
    ALLEGRO_BITMAP* imagen_walk1;
    ALLEGRO_BITMAP* imagen_walk2;
    ALLEGRO_BITMAP* imagen_walk3;
    ALLEGRO_BITMAP* imagen_walk4;
    ALLEGRO_BITMAP* imagen_golpe1[13];  // Array para las imágenes de la primera animación de golpe
    ALLEGRO_BITMAP* imagen_golpe2[11];  // Array para las imágenes de la segunda animación de golpe
    float x, y;
    float escala_x, escala_y;
    bool en_movimiento;
    bool mirando_izquierda;
    bool golpeando;
    int animacion_golpe;  // Alternar entre las animaciones de golpe
    int frame_actual;
    int frame_golpe;

    Player(const char* ruta_idle1, const char* ruta_idle2, const char* ruta_walk1, const char* ruta_walk2, const char* ruta_walk3, const char* ruta_walk4, const char* rutas_golpe1[], const char* rutas_golpe2[], float pantalla_ancho, float pantalla_alto, float escala_x, float escala_y);
    ~Player();
    void dibujar();
    void cambiar_imagen_idle();
    void cambiar_imagen_walk();
    void cambiar_imagen_golpe();
    void mover(float dx, float dy);
    void golpear();
};

#endif
