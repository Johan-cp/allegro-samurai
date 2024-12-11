#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "player/player.h"
#include "zombie/zombie.h"

#include <vector>
#include <memory>
#include <iostream>

bool inicializar_allegro()
{
    if (!al_init() || !al_install_keyboard() || !al_init_image_addon() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon() || !al_install_audio() || !al_init_acodec_addon())
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo inicializar Allegro", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    if (!al_reserve_samples(1))
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo reservar canales de audio", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }
    return true;
}

ALLEGRO_DISPLAY *crear_display(float pantalla_ancho, float pantalla_alto)
{
    ALLEGRO_DISPLAY *display = al_create_display(pantalla_ancho, pantalla_alto);
    if (!display)
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo crear la pantalla", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    return display;
}

ALLEGRO_FONT *cargar_fuente(const char *ruta, int tamano)
{
    ALLEGRO_FONT *font = al_load_font(ruta, tamano, 0);
    if (!font)
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la fuente", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    return font;
}

void reiniciar_juego(Player &player, std::vector<std::shared_ptr<Zombie>> &zombies, int &puntaje, float pantalla_ancho, float pantalla_alto)
{
    player.x = pantalla_ancho / 2 - (al_get_bitmap_width(player.imagen_idle1) * player.escala_x) / 2;
    player.y = pantalla_alto / 2 - (al_get_bitmap_height(player.imagen_idle1) * player.escala_y) / 2;
    player.en_movimiento = false;
    player.golpeando = false;
    player.frame_actual = 0;
    player.frame_golpe = 0;
    player.animacion_golpe = 0;
    zombies.clear();
    puntaje = 0;
    std::cout << "Juego reiniciado.\n";
}

int main()
{
    if (!inicializar_allegro())
    {
        return -1;
    }

    float pantalla_ancho = 900;
    float pantalla_alto = 700;
    ALLEGRO_DISPLAY *display = crear_display(pantalla_ancho, pantalla_alto);
    if (!display)
    {
        return -1;
    }

    ALLEGRO_FONT *font = cargar_fuente("./fonts/ProsperoBoldNbpRegular-ZZRB.ttf", 24);
    if (!font)
    {
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_FONT *font_grande = cargar_fuente("./fonts/ProsperoBoldNbpRegular-ZZRB.ttf", 48);
    if (!font_grande)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        return -1;
    }

    ALLEGRO_BITMAP *fondo = al_load_bitmap("./assets/fondo.png");
    if (!fondo)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        al_destroy_font(font_grande);
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la imagen de fondo", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    ALLEGRO_SAMPLE *sonido_golpe = al_load_sample("./sounds/sword-sound.wav");
    if (!sonido_golpe)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        al_destroy_font(font_grande);
        al_destroy_bitmap(fondo);
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar el sonido de golpe", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    const char *rutas_golpe1[13] = {
        "./player/assets/first_punch/punch_0.png",
        "./player/assets/first_punch/punch_1.png",
        "./player/assets/first_punch/punch_2.png",
        "./player/assets/first_punch/punch_3.png",
        "./player/assets/first_punch/punch_4.png",
        "./player/assets/first_punch/punch_5.png",
        "./player/assets/first_punch/punch_6.png",
        "./player/assets/first_punch/punch_7.png",
        "./player/assets/first_punch/punch_8.png",
        "./player/assets/first_punch/punch_9.png",
        "./player/assets/first_punch/punch_10.png",
        "./player/assets/first_punch/punch_11.png",
        "./player/assets/first_punch/punch_12.png"};

    const char *rutas_golpe2[11] = {
        "./player/assets/second_punch/punch_0.png",
        "./player/assets/second_punch/punch_1.png",
        "./player/assets/second_punch/punch_2.png",
        "./player/assets/second_punch/punch_3.png",
        "./player/assets/second_punch/punch_4.png",
        "./player/assets/second_punch/punch_5.png",
        "./player/assets/second_punch/punch_6.png",
        "./player/assets/second_punch/punch_7.png",
        "./player/assets/second_punch/punch_8.png",
        "./player/assets/second_punch/punch_9.png",
        "./player/assets/second_punch/punch_10.png"};

    Player player("./player/assets/player.png", "./player/assets/player_2.png", "./player/assets/player.png", "./player/assets/player.png", "./player/assets/player_walk.png", "./player/assets/player_walk.png", rutas_golpe1, rutas_golpe2, pantalla_ancho, pantalla_alto, 2.0, 2.0);
    std::vector<std::shared_ptr<Zombie>> zombies;
    int puntaje = 0;
    bool game_over = false;

    // Variable para el color del fondo
    bool fondo_negro = false;

    // Temporizador para la animación idle
    ALLEGRO_TIMER *idle_timer = al_create_timer(0.5);
    al_register_event_source(event_queue, al_get_timer_event_source(idle_timer));
    al_start_timer(idle_timer);

    // Temporizador para el movimiento
    ALLEGRO_TIMER *move_timer = al_create_timer(0.02); // Ajustar a 0.02 segundos para 50 fps
    al_register_event_source(event_queue, al_get_timer_event_source(move_timer));
    al_start_timer(move_timer);

    // Temporizador para la animación de golpe
    ALLEGRO_TIMER *golpe_timer = al_create_timer(0.05); // Ajustar a 0.05 segundos para una animación de golpe más rápida
    al_register_event_source(event_queue, al_get_timer_event_source(golpe_timer));
    al_start_timer(golpe_timer);

    std::cout << "Timers inicializados.\n";

    bool running = true;
    bool key[4] = {false, false, false, false}; // W, S, A, D

    while (running)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
            std::cout << "Cerrando el juego.\n";
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (game_over)
            {
                if (event.keyboard.keycode == ALLEGRO_KEY_R)
                {
                    game_over = false;
                    reiniciar_juego(player, zombies, puntaje, pantalla_ancho, pantalla_alto);
                    fondo_negro = false; // Volver a fondo normal
                }
            }
            else
            {
                switch (event.keyboard.keycode)
                {
                case ALLEGRO_KEY_W:
                    key[0] = true;
                    break;
                case ALLEGRO_KEY_S:
                    key[1] = true;
                    break;
                case ALLEGRO_KEY_A:
                    key[2] = true;
                    break;
                case ALLEGRO_KEY_D:
                    key[3] = true;
                    break;
                case ALLEGRO_KEY_J: // Tecla para golpear
                    player.golpear();
                    al_play_sample(sonido_golpe, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Reproducir sonido
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    running = false;
                    std::cout << "Juego cerrado por el usuario.\n";
                    break;
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (event.keyboard.keycode)
            {
            case ALLEGRO_KEY_W:
                key[0] = false;
                break;
            case ALLEGRO_KEY_S:
                key[1] = false;
                break;
            case ALLEGRO_KEY_A:
                key[2] = false;
                break;
            case ALLEGRO_KEY_D:
                key[3] = false;
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_TIMER)
        {
            if (event.timer.source == move_timer)
            {
                float dx = 0, dy = 0;

                if (key[0])
                    dy -= 5; // Reducir la velocidad de movimiento vertical
                if (key[1])
                    dy += 5; // Reducir la velocidad de movimiento vertical
                if (key[2])
                    dx -= 5; // Reducir la velocidad de movimiento horizontal
                if (key[3])
                    dx += 5; // Reducir la velocidad de movimiento horizontal

                player.mover(dx, dy);

                if (!key[0] && !key[1] && !key[2] && !key[3])
                {
                    player.en_movimiento = false;
                }
                else
                {
                    player.en_movimiento = true;
                    player.cambiar_imagen_walk(); // Cambiar la imagen de caminar mientras se mueve
                }

                // Mover y dibujar zombies
                for (auto &zombie : zombies)
                {
                    zombie->mover(player.x, player.y);
                }
            }
            else if (event.timer.source == idle_timer)
            {
                if (!player.en_movimiento && !player.golpeando)
                {
                    player.cambiar_imagen_idle();
                }
            }
            else if (event.timer.source == golpe_timer)
            {
                if (player.golpeando)
                {
                    player.cambiar_imagen_golpe(); // Cambiar la imagen de golpe durante la animación de golpe
                    for (auto &zombie : zombies)
                    {
                        if (zombie->colision(player.x, player.y, 50, 50))
                        { // Ajustar el tamaño del jugador
                            zombie->vivo = false;
                            puntaje += 1;
                            std::cout << "Zombie eliminado. Puntaje: " << puntaje << "\n";
                        }
                    }
                }
            }

            // Comprobar colisiones con zombies
            if (!player.golpeando)
            {
                for (auto &zombie : zombies)
                {
                    if (zombie->colision(player.x, player.y, 50, 50))
                    { // Ajustar el tamaño del jugador
                        game_over = true;
                        fondo_negro = true; // Cambiar el fondo a negro cuando el juego termine
                        std::cout << "Colisión con zombie. GAME OVER.\n";
                        break;
                    }
                }
            }
        }

        // Dibujar fondo
        if (fondo_negro)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
        else
        {
            al_draw_bitmap(fondo, 0, 0, 0);
        }

        if (game_over)
        {
            al_draw_text(font_grande, al_map_rgb(255, 0, 0), pantalla_ancho / 2, pantalla_alto / 2 - 48, ALLEGRO_ALIGN_CENTER, "GAME OVER");
            al_draw_textf(font, al_map_rgb(255, 255, 255), pantalla_ancho / 2, pantalla_alto / 2, ALLEGRO_ALIGN_CENTER, "Puntaje: %d", puntaje);
            al_draw_text(font, al_map_rgb(255, 255, 255), pantalla_ancho / 2, pantalla_alto / 2 + 48, ALLEGRO_ALIGN_CENTER, "Presiona R para reiniciar");
        }
        else
        {
            // Dibujar zombies
            for (auto &zombie : zombies)
            {
                zombie->dibujar();
            }

            // Dibujar jugador
            player.dibujar();

            // Dibujar puntaje
            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Puntaje: %d", puntaje);

            // Agregar nuevos zombies aleatoriamente
            if (rand() % 100 < 2)
            { // Controlar la frecuencia de aparición
                zombies.push_back(std::make_shared<Zombie>(pantalla_ancho, pantalla_alto, "./zombie/assets/zombie.png"));
                std::cout << "Zombie agregado. Total de zombies: " << zombies.size() << "\n";
            }
        }

        al_flip_display();
    }

    al_destroy_bitmap(fondo);
    al_destroy_timer(idle_timer);
    al_destroy_timer(move_timer);
    al_destroy_timer(golpe_timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_font(font);
    al_destroy_font(font_grande);
    al_destroy_sample(sonido_golpe);

    std::cout << "Recursos liberados. Juego terminado.\n";

    return 0;
}
