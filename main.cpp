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

bool start_allegro()
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

ALLEGRO_DISPLAY *create_display(float screen_width, float screen_height)
{
    ALLEGRO_DISPLAY *display = al_create_display(screen_width, screen_height);
    if (!display)
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo crear la pantalla", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    return display;
}

ALLEGRO_FONT *load_fonts(const char *ruta, int tamano)
{
    ALLEGRO_FONT *font = al_load_font(ruta, tamano, 0);
    if (!font)
    {
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la fuente", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
    return font;
}

void reiniciar_juego(Player &player, std::vector<std::shared_ptr<Zombie>> &zombies, int &score, float screen_width, float screen_height)
{
    player.x = screen_width / 2 - (al_get_bitmap_width(player.images_idle1) * player.scale_x) / 2;
    player.y = screen_height / 2 - (al_get_bitmap_height(player.images_idle1) * player.scale_y) / 2;
    player.in_movement = false;
    player.puching = false;
    player.actual_frame = 0;
    player.punch_frame = 0;
    player.punch_animation = 0;
    zombies.clear();
    score = 0;
    std::cout << "Juego reiniciado.\n";
}

int main()
{
    if (!start_allegro())
    {
        return -1;
    }

    float screen_width = 900;
    float screen_height = 700;
    ALLEGRO_DISPLAY *display = create_display(screen_width, screen_height);
    if (!display)
    {
        return -1;
    }

    ALLEGRO_FONT *font = load_fonts("./fonts/ProsperoBoldNbpRegular-ZZRB.ttf", 24);
    if (!font)
    {
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_FONT *game_over_font = load_fonts("./fonts/ProsperoBoldNbpRegular-ZZRB.ttf", 48);
    if (!game_over_font)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        return -1;
    }

    ALLEGRO_BITMAP *background = al_load_bitmap("./assets/background.png");
    if (!background)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        al_destroy_font(game_over_font);
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar la imagen de fondo", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    ALLEGRO_SAMPLE *punch_sound = al_load_sample("./sounds/sword-sound.wav");
    if (!punch_sound)
    {
        al_destroy_display(display);
        al_destroy_font(font);
        al_destroy_font(game_over_font);
        al_destroy_bitmap(background);
        al_show_native_message_box(NULL, "Error", "Error", "No se pudo cargar el sonido de golpe", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    const char *route_punch1[13] = {
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

    const char *route_punch2[11] = {
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

    Player player("./player/assets/player.png", "./player/assets/player_2.png", "./player/assets/player.png", "./player/assets/player.png", "./player/assets/player_walk.png", "./player/assets/player_walk.png", route_punch1, route_punch2, screen_width, screen_height, 2.0, 2.0);
    std::vector<std::shared_ptr<Zombie>> zombies;
    int score = 0;
    bool game_over = false;

    bool black_background = false;

    ALLEGRO_TIMER *idle_timer = al_create_timer(0.5);
    al_register_event_source(event_queue, al_get_timer_event_source(idle_timer));
    al_start_timer(idle_timer);

    ALLEGRO_TIMER *move_timer = al_create_timer(0.02);
    al_register_event_source(event_queue, al_get_timer_event_source(move_timer));
    al_start_timer(move_timer);

    ALLEGRO_TIMER *golpe_timer = al_create_timer(0.05);
    al_register_event_source(event_queue, al_get_timer_event_source(golpe_timer));
    al_start_timer(golpe_timer);

    std::cout << "Timers inicializados.\n";

    bool running = true;
    bool key[4] = {false, false, false, false};

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
                    reiniciar_juego(player, zombies, score, screen_width, screen_height);
                    black_background = false;
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
                case ALLEGRO_KEY_J:
                    player.punch();
                    al_play_sample(punch_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
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
                    dy -= 5;
                if (key[1])
                    dy += 5;
                if (key[2])
                    dx -= 5;
                if (key[3])
                    dx += 5;

                player.move(dx, dy);

                if (!key[0] && !key[1] && !key[2] && !key[3])
                {
                    player.in_movement = false;
                }
                else
                {
                    player.in_movement = true;
                    player.change_images_walk();
                }

                for (auto &zombie : zombies)
                {
                    zombie->move(player.x, player.y);
                }
            }
            else if (event.timer.source == idle_timer)
            {
                if (!player.in_movement && !player.puching)
                {
                    player.change_images_idle();
                }
            }
            else if (event.timer.source == golpe_timer)
            {
                if (player.puching)
                {
                    player.change_images_punch();
                    for (auto &zombie : zombies)
                    {
                        if (zombie->collision(player.x, player.y, 50, 50))
                        {
                            zombie->isAlive = false;
                            score += 1;
                            std::cout << "Zombie eliminado. Puntaje: " << score << "\n";
                        }
                    }
                }
            }

            if (!player.puching)
            {
                for (auto &zombie : zombies)
                {
                    if (zombie->collision(player.x, player.y, 50, 50))
                    {
                        game_over = true;
                        black_background = true;
                        std::cout << "Colisión con zombie. GAME OVER.\n";
                        break;
                    }
                }
            }
        }

        if (black_background)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
        }
        else
        {
            al_draw_bitmap(background, 0, 0, 0);
        }

        if (game_over)
        {
            al_draw_text(game_over_font, al_map_rgb(255, 0, 0), screen_width / 2, screen_height / 2 - 48, ALLEGRO_ALIGN_CENTER, "GAME OVER");
            al_draw_textf(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2, ALLEGRO_ALIGN_CENTER, "Puntaje: %d", score);
            al_draw_text(font, al_map_rgb(255, 255, 255), screen_width / 2, screen_height / 2 + 48, ALLEGRO_ALIGN_CENTER, "Presiona R para reiniciar");
        }
        else
        {
            for (auto &zombie : zombies)
            {
                zombie->draw();
            }

            player.draw();

            al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Puntaje: %d", score);

            if (rand() % 100 < 2)
            {
                zombies.push_back(std::make_shared<Zombie>(screen_width, screen_height, "./zombie/assets/zombie.png"));
                std::cout << "Zombie agregado. Total de zombies: " << zombies.size() << "\n";
            }
        }

        al_flip_display();
    }

    al_destroy_bitmap(background);
    al_destroy_timer(idle_timer);
    al_destroy_timer(move_timer);
    al_destroy_timer(golpe_timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_font(font);
    al_destroy_font(game_over_font);
    al_destroy_sample(punch_sound);

    std::cout << "Recursos liberados. Juego terminado.\n";

    return 0;
}
