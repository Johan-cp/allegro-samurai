#include "zombie.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

Zombie::Zombie(float pantalla_ancho, float pantalla_alto, const char* ruta_imagen) {
    imagen = std::shared_ptr<ALLEGRO_BITMAP>(al_load_bitmap(ruta_imagen), al_destroy_bitmap);
    if (!imagen) {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la imagen del zombie", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    } else {
        std::cout << "Imagen del zombie cargada correctamente.\n";
    }
    ancho = al_get_bitmap_width(imagen.get());
    alto = al_get_bitmap_height(imagen.get());
    vivo = true;

    // Inicializar la posición aleatoriamente a la izquierda o derecha del mapa
    if (rand() % 2 == 0) {
        x = 0 - ancho;  // Aparece a la izquierda del mapa
    } else {
        x = pantalla_ancho + ancho;  // Aparece a la derecha del mapa
    }
    y = rand() % static_cast<int>(pantalla_alto);
    std::cout << "Zombie creado en la posición (" << x << ", " << y << ").\n";
}

void Zombie::dibujar() {
    if (vivo) {
        float escala = 2.0;  // Factor de escala para hacer la imagen más grande
        al_draw_scaled_bitmap(imagen.get(), 0, 0, ancho, alto, x, y, ancho * escala, alto * escala, 0);
    }
}

void Zombie::mover(float jugador_x, float jugador_y) {
    if (vivo) {
        float dx = jugador_x - x;
        float dy = jugador_y - y;
        float distancia = sqrt(dx * dx + dy * dy);
        x += (dx / distancia) * 1.0;  // Velocidad del zombie hacia el jugador
        y += (dy / distancia) * 1.0;
    }
}

bool Zombie::colision(float jugador_x, float jugador_y, float jugador_ancho, float jugador_alto) {
    return vivo && x + ancho > jugador_x && x < jugador_x + jugador_ancho &&
           y + alto > jugador_y && y < jugador_y + jugador_alto;
}
