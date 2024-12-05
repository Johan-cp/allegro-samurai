#ifndef ZOMBIE_H
#define ZOMBIE_H

#include <allegro5/allegro.h>
#include <memory>

class Zombie {
public:
    float x, y;
    float ancho, alto;
    bool vivo;
    std::shared_ptr<ALLEGRO_BITMAP> imagen;  // Imagen del zombie con puntero inteligente

    Zombie(float pantalla_ancho, float pantalla_alto, const char* ruta_imagen);
    void dibujar();
    void mover(float jugador_x, float jugador_y);
    bool colision(float jugador_x, float jugador_y, float jugador_ancho, float jugador_alto);
};

#endif
