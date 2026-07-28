#include "sounds.h"
#include "items.h"
#include "def.h"
#include "tools.h"
#include "graph.h"

ALLEGRO_SAMPLE* test_sound;
ALLEGRO_SAMPLE* snd_jump;
ALLEGRO_SAMPLE* snd_sjump;
ALLEGRO_SAMPLE* snd_bump;
ALLEGRO_SAMPLE* snd_brick_break;
ALLEGRO_SAMPLE* snd_block_break;
ALLEGRO_SAMPLE* snd_enter_water;
ALLEGRO_SAMPLE* snd_exit_water;
ALLEGRO_SAMPLE* snd_explode;
ALLEGRO_SAMPLE* snd_get_item;
ALLEGRO_SAMPLE* snd_finish;
ALLEGRO_SAMPLE* snd_teleport;
ALLEGRO_SAMPLE* snd_mine;
ALLEGRO_SAMPLE* snd_speed_start;
ALLEGRO_SAMPLE* snd_speed_end;
ALLEGRO_SAMPLE* snd_bounce;
ALLEGRO_SAMPLE* snd_happy;
ALLEGRO_SAMPLE* snd_sad;
ALLEGRO_SAMPLE* snd_jetpack;
ALLEGRO_SAMPLE* snd_laser_shoot;
ALLEGRO_SAMPLE* snd_laser_hit;
ALLEGRO_SAMPLE* snd_sword;

ALLEGRO_AUDIO_STREAM* music;
Pointer_list* music_files_list;

bool mute = false;
static int2 sound_played_at = {0, 0};
bool jetpack_sound_playing = false;
ALLEGRO_SAMPLE_ID jetpack_sample_id;
ALLEGRO_SAMPLE_ID playing_music_id;
bool is_music_playing;

void reset_sound_played_at()
{
	sound_played_at = (int2) {0, 0};
}

void init_audio()
{
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(30);
	music_files_list = get_directory_contents("assets/music");
    for(int i=0; i<music_files_list->elm_counter; i++)
    {
        char* track_name = copy_str(ptrl_get_pointer(music_files_list, i));
        track_name[strlen(track_name)-4] = 0;        
        add_item_to_gui_list(gui_list_music, track_name);
        free(track_name);       
    }
}

void play_bump_sound(int x, int y, int type)
{
	if(type == 0) return;

    if(sound_played_at.x != x || sound_played_at.y != y)
    {
        play_sound(snd_bump);
        sound_played_at = (int2) {x, y};
    }
}

void reset_bump_sound()
{
    sound_played_at = (int2) {-1, -1};
}

void load_sounds()
{
    snd_jump = al_load_sample("assets/sounds/jump.wav");
    snd_sjump = al_load_sample("assets/sounds/super_jump.wav");
    snd_bump = al_load_sample("assets/sounds/bump.wav");
    snd_brick_break = al_load_sample("assets/sounds/brick_break_pr1.wav");
    snd_block_break = al_load_sample("assets/sounds/block_break.wav");
    snd_enter_water = al_load_sample("assets/sounds/enter_water.wav");
    snd_exit_water = al_load_sample("assets/sounds/exit_water.wav");
    snd_explode = al_load_sample("assets/sounds/explode.wav");
    snd_get_item = al_load_sample("assets/sounds/get_item.wav");
    snd_finish = al_load_sample("assets/sounds/finish.wav");
    snd_teleport = al_load_sample("assets/sounds/teleport.wav");
    snd_mine = al_load_sample("assets/sounds/mine.wav");
    snd_speed_start = al_load_sample("assets/sounds/speed_start.wav");
    snd_speed_end = al_load_sample("assets/sounds/speed_end.wav");
    snd_bounce = al_load_sample("assets/sounds/bounce.wav");
    snd_happy = al_load_sample("assets/sounds/happy.wav");
    snd_sad = al_load_sample("assets/sounds/sad.wav");
    snd_jetpack = al_load_sample("assets/sounds/jetpack.wav");
    snd_laser_shoot = al_load_sample("assets/sounds/laser_shoot.wav");
    snd_laser_hit = al_load_sample("assets/sounds/laser_hit.wav");
    snd_sword = al_load_sample("assets/sounds/sword.wav");

    //music[0] = al_load_sample("assets/music/instrumental4.ogg");
    //music[1] = al_load_sample("assets/music/extracted realms.ogg");
    
}

void play_sound(ALLEGRO_SAMPLE* sound)
{
    if(!mute)
    {
        al_play_sample(sound, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, 0);
        
    }
}

void play_music(int id)
{
    if(!mute)
    {    
        if(id == 0)
        {
            if(music != NULL)
            {
                al_set_audio_stream_playing(music, false);
                al_destroy_audio_stream(music);
                music = NULL;
            }
            return;
        }

        if(music != NULL)
        {          
            al_set_audio_stream_playing(music, false);
            al_destroy_audio_stream(music);
            music = NULL;
        }        
        if(music_files_list == NULL)
        {
            return;
        }
        char* track_name = ptrl_get_pointer(music_files_list, id-1);        
        char filename[64];

        sprintf(filename, "assets/music/%s", track_name);        
        music = al_load_audio_stream(filename, 4, 2048);
        if(music != NULL)
        {
            al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
            al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
            al_set_audio_stream_playing(music, true);
        }
    }
}

/*void handle_jetpack_sound(Player* pl)
{
    if(KEY_DOWN(ALLEGRO_KEY_SPACE))
    {
        if(pl->held_item == ITEM_JETPACK &! jetpack_sound_playing)
        {
            al_play_sample(snd_jetpack, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, &jetpack_sample_id);
            jetpack_sound_playing = true;
        }
    }
    else if(jetpack_sound_playing)
    {
        al_stop_sample(&jetpack_sample_id);
        jetpack_sound_playing = false;
    }
    if(pl->held_item != ITEM_JETPACK && jetpack_sound_playing)
    {
        al_stop_sample(&jetpack_sample_id);
        jetpack_sound_playing = false;
    }
}*/









