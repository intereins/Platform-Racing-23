#ifndef SOUNDS_H
#define SOUNDS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

extern ALLEGRO_SAMPLE* test_sound;
extern ALLEGRO_SAMPLE* snd_jump;
extern ALLEGRO_SAMPLE* snd_sjump;
extern ALLEGRO_SAMPLE* snd_bump;
extern ALLEGRO_SAMPLE* snd_brick_break;
extern ALLEGRO_SAMPLE* snd_block_break;
extern ALLEGRO_SAMPLE* snd_enter_water;
extern ALLEGRO_SAMPLE* snd_exit_water;
extern ALLEGRO_SAMPLE* snd_explode;
extern ALLEGRO_SAMPLE* snd_get_item;
extern ALLEGRO_SAMPLE* snd_finish;
extern ALLEGRO_SAMPLE* snd_teleport;
extern ALLEGRO_SAMPLE* snd_mine;
extern ALLEGRO_SAMPLE* snd_speed_start;
extern ALLEGRO_SAMPLE* snd_speed_end;
extern ALLEGRO_SAMPLE* snd_bounce;
extern ALLEGRO_SAMPLE* snd_happy;
extern ALLEGRO_SAMPLE* snd_sad;
extern ALLEGRO_SAMPLE* snd_jetpack;
extern ALLEGRO_SAMPLE* snd_laser_shoot;
extern ALLEGRO_SAMPLE* snd_laser_hit;
extern ALLEGRO_SAMPLE* snd_sword;

extern bool mute;
extern const bool play_bump_snd[29];
struct Player;

void reset_sound_played_at();
void init_audio();
void play_bump_sound(int x, int y, int type);
void reset_bump_sound();
void load_sounds();

void play_sound(ALLEGRO_SAMPLE* sound);
void play_music(int id);
void handle_jetpack_sound(struct Player *pl);

#endif // SOUNDS_H
