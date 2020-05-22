#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.h>

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768
#define GAMESTATE start

sfEvent           event;

enum gamestate { pause, start, game };
enum gamestate gs;
sfRenderWindow   *window;
sfVector2u        ws;
sfView           *gameview;
double            fallspeed;
double            flyspeed;

int  init                 (void); 
int  run                  (void); 
void loop                 (void); 
void render               (void); 
void game_render          (void); 
void start_render         (void); 
void pause_render         (void); 
void background_render    (void); 
void update               (int); 
void start_update         (void); 
void pause_update         (void); 
void pause_animation      (void); 
void game_update          (void); 
void background_update    (void); 
void bird_update          (void); 
void stris_update         (void); 
void stris_animation      (void); 
void colcheck_update      (void); 
void gui_update           (void); 
void game_close           (void); 
void make_triangles       (void); 

#endif
