#include <math.h>
#include "level.h"
#include "graph.h"
#include "player.h"
#include "tools.h"
#include "block.h"
#include "map.h"
#include "art.h"
#include "render.h"
#include "sounds.h"

Level* the_level = NULL;
const number_t g_default_gravity = 0.7;
enum Rotation current_rotation = ROT_0;

Level* create_level()
{
    Level* lvl = calloc(1, sizeof(Level));
    lvl->chunk_list = exl_create(10, sizeof(Chunk), 2);
    init_level_art(lvl);
    lvl->camera_x = 444*30;
    lvl->camera_y = 335*30;
    lvl->gravity_multiplier = 1;
    lvl->bgcol = (byte3) {.x = 0xBA, .y = 0xBA, .z = 0xDC};
    lvl->start_pos[0].x = 444;
    lvl->start_pos[0].y = 330;
    the_player.safe_x = 444*30;
    the_player.safe_y = 330*30;
    return lvl;
}

void destroy_level(Level* lvl)
{
    if(lvl != NULL)
    {
        gtex_destroy_texture(lvl->map);
        exl_destroy(lvl->layer_list);
        exl_destroy(lvl->chunk_list);
        free(lvl);
    }
}

int get_block_chunk_index(Extended_list* chunk_list, int x, int y)
{
    for(int i=0; i<chunk_list->elm_count; i++)
    {
        Chunk* chunk = exl_get_elm_ptr(chunk_list, i);
        if(chunk->x == x && chunk->y == y)
        {
            return i;
        }
    }
    return -1;
}

int2 rotate_position(int x, int y, enum Rotation rot, int square_size)
{
    switch(rot)
    {
        case ROT_0: return (int2) {.x=x, .y=y};
        case ROT_90: return (int2) {.x = square_size-y, .y=x};
        case ROT_180: return (int2) {.x = square_size-x, .y = square_size-y};
        case ROT_270: return (int2) {.x = y, .y = square_size-x};
    }
}

void set_block(Level* lvl, int x, int y, int type)
{
    if(type != 0 && block_table[type].type == BT_START)
    {
        lvl->start_pos[0].x = x;
        lvl->start_pos[0].y = y;
    }

    int2 rot_pos = rotate_position(x, y, (4-current_rotation)%4, 1999);
    x = rot_pos.x;
    y = rot_pos.y;

    int chunk_x = x / BLOCK_CHUNK_SZ;
    int chunk_y = y / BLOCK_CHUNK_SZ;

    Chunk* chunk;
    int index = get_block_chunk_index(lvl->chunk_list, chunk_x, chunk_y);
    if(index == -1)
    {
        index = add_chunk_to_level(lvl, chunk_x, chunk_y);
    }
    chunk = exl_get_elm_ptr(lvl->chunk_list, index);
    set_block_on_chunk(chunk, x % BLOCK_CHUNK_SZ, y % BLOCK_CHUNK_SZ, type);
}

void set_block_ptr(Level* lvl, int x, int y, Block* ptr)
{
    int2 rot_pos = rotate_position(x, y, (4-current_rotation)%4, 1999);
    x = rot_pos.x;
    y = rot_pos.y;

    int chunk_x = x / BLOCK_CHUNK_SZ;
    int chunk_y = y / BLOCK_CHUNK_SZ;

    Chunk* chunk;
    int index = get_block_chunk_index(lvl->chunk_list, chunk_x, chunk_y);
    if(index == -1)
    {
        index = add_chunk_to_level(lvl, chunk_x, chunk_y);
    }
    chunk = exl_get_elm_ptr(lvl->chunk_list, index);
    free_block(chunk->block[y % BLOCK_CHUNK_SZ][x % BLOCK_CHUNK_SZ]);
    chunk->block[y % BLOCK_CHUNK_SZ][x % BLOCK_CHUNK_SZ] = ptr;
}

Block* get_block(Level* lvl, int x, int y)
{
    int2 rot_pos = rotate_position(x, y, (4-current_rotation)%4, 1999);
    x = rot_pos.x;
    y = rot_pos.y;

    int chunk_x = x / BLOCK_CHUNK_SZ;
    int chunk_y = y / BLOCK_CHUNK_SZ;

    Chunk* chunk;
    int index = get_block_chunk_index(lvl->chunk_list, chunk_x, chunk_y);
    if(index == -1)
    {
        return NULL;
    }
    chunk = exl_get_elm_ptr(lvl->chunk_list, index);
    return get_block_from_chunk(chunk, x % BLOCK_CHUNK_SZ, y % BLOCK_CHUNK_SZ);
}

Block* pr2_get_block(number_t pos_x, number_t pos_y, bool b1, bool b2)
{
    Level* lvl = the_level;
    int2 rot_pos = rotate_position(pos_x/BLOCK_W, pos_y/BLOCK_H, (4-current_rotation)%4, 1999);
    pos_x = rot_pos.x*BLOCK_W;
    pos_y = rot_pos.y*BLOCK_H;

    int x = floor(pos_x / BLOCK_W);
    int y = floor(pos_y / BLOCK_H);
    int chunk_x = x / BLOCK_CHUNK_SZ;
    int chunk_y = y / BLOCK_CHUNK_SZ;

    Chunk* chunk;
    int index = get_block_chunk_index(lvl->chunk_list, chunk_x, chunk_y);
    if(index == -1)
    {
        return NULL;
    }
    chunk = exl_get_elm_ptr(lvl->chunk_list, index);
    Block* ret = get_block_from_chunk(chunk, x % BLOCK_CHUNK_SZ, y % BLOCK_CHUNK_SZ);
    if(ret == NULL) return NULL;

    if(ret->can_vanish && ret->vanish_timer >= ret->vanish_len1)
    {
        return NULL;
    }
    return ret;
}

Block* pr2_get_block_from_pos(number_t pos_x, number_t pos_y, bool b1)
{
    Level* lvl = the_level;
    int2 rot_pos = rotate_position(pos_x/BLOCK_W, pos_y/BLOCK_H, (4-current_rotation)%4, 1999);
    pos_x = rot_pos.x*BLOCK_W;
    pos_y = rot_pos.y*BLOCK_H;

    int x = floor(pos_x / BLOCK_W);
    int y = floor(pos_y / BLOCK_H);
    int chunk_x = x / BLOCK_CHUNK_SZ;
    int chunk_y = y / BLOCK_CHUNK_SZ;

    Chunk* chunk;
    int index = get_block_chunk_index(lvl->chunk_list, chunk_x, chunk_y);
    if(index == -1)
    {
        return NULL;
    }
    chunk = exl_get_elm_ptr(lvl->chunk_list, index);
    return get_block_from_chunk(chunk, x % BLOCK_CHUNK_SZ, y % BLOCK_CHUNK_SZ);
}

void render_level(Level* lvl, number_t screen_x, number_t screen_y)
{
    render_color_rect(0, 0, DEF_WIN_W, DEF_WIN_H, lvl->bgcol.x/255.0, lvl->bgcol.y/255.0, lvl->bgcol.z/255.0, 1);
    render_level_art(lvl, &the_player, -screen_x, -screen_y);

    for(int i=0; i<lvl->chunk_list->elm_count; i++)
    {
        Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
        int2 rot_pos = rotate_position(chunk->x, chunk->y, current_rotation, 2000/BLOCK_CHUNK_SZ - 1);
        render_chunk(chunk, rot_pos.x*BLOCK_CHUNK_SZ*BLOCK_W + screen_x, rot_pos.y*BLOCK_CHUNK_SZ*BLOCK_H + screen_y);
    }

    if(lvl->map != NULL)
    {
        render_level_map(lvl, &the_player);
    }
}

void update_level(Level* lvl)
{
    for(int i=0; i<lvl->chunk_list->elm_count; i++)
    {
        Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
        update_chunk(chunk);
    }
}

void update_block_offsets()
{
    for(int i=0; i<the_level->chunk_list->elm_count; i++)
    {
        Chunk* chunk = exl_get_elm_ptr(the_level->chunk_list, i);
        update_block_offsets_on_chunk(chunk);
    }
}

int get_level_max_y()
{
    switch(current_rotation)
    {
        case ROT_0: return the_level->map_border_y2;
        case ROT_90: return the_level->map_border_x2;
        case ROT_180: return 1999 - the_level->map_border_y1;
        case ROT_270: return 1999 - the_level->map_border_x1;
    }
}

int add_chunk_to_level(Level* lvl, int x, int y)
{
    Chunk* chunk = exl_add_get_ptr(lvl->chunk_list);
    chunk->x = x;
    chunk->y = y;
    memset(chunk->block, 0, sizeof(Block*)*BLOCK_CHUNK_SZ*BLOCK_CHUNK_SZ);
    return lvl->chunk_list->elm_count - 1;
}

Chunk* create_chunk(int x, int y)
{
    Chunk* ret = calloc(sizeof(Chunk), 1);
    ret->x = x;
    ret->y = y;
    return ret;
}

void render_chunk(Chunk* chunk, number_t screen_x, number_t screen_y)
{
    if(screen_x < -BLOCK_CHUNK_SZ*BLOCK_W) return;
    if(screen_y < -BLOCK_CHUNK_SZ*BLOCK_W) return;
    if(screen_x > DEF_WIN_W) return;
    if(screen_y > DEF_WIN_H) return;

    for(int y=0; y<BLOCK_CHUNK_SZ; y++)
    {
        for(int x=0; x<BLOCK_CHUNK_SZ; x++)
        {
            int2 rot_pos = rotate_position(x, y, current_rotation, BLOCK_CHUNK_SZ-1);
            Block* block = chunk->block[y][x];
            if(block == NULL)
            {
                continue;
            }
            block_render(block, screen_x+rot_pos.x*BLOCK_W, screen_y+rot_pos.y*BLOCK_H, the_player.rotating);
        }
    }
}

void update_chunk(Chunk* chunk)
{
    for(int y=0; y<BLOCK_CHUNK_SZ; y++)
    {
        for(int x=0; x<BLOCK_CHUNK_SZ; x++)
        {
            Block* block = chunk->block[y][x];
            if(block == NULL)
            {
                continue;
            }
            block_update(block);
        }
    }
}

void update_block_offsets_on_chunk(Chunk* chunk)
{
    for(int y=0; y<BLOCK_CHUNK_SZ; y++)
    {
        for(int x=0; x<BLOCK_CHUNK_SZ; x++)
        {
            Block* block = chunk->block[y][x];
            if(block == NULL)
            {
                continue;
            }
            update_block_offset(block);
        }
    }
}

void free_block(Block* block)
{
    free(block);
}

void set_block_on_chunk(Chunk* chunk, int x, int y, int type)
{
    if(type == 0)
    {
        free_block(chunk->block[y][x]);
        chunk->block[y][x] = NULL;
    }
    else
    {
        chunk->block[y][x] = calloc(1, sizeof(Block));
        Block* block = chunk->block[y][x];

        block->active = false;
        block->block_code = type;
        block->removed = false;
        block->chunk_x = chunk->x;
        block->chunk_y = chunk->y;
        block->pos_x = (chunk->x*BLOCK_CHUNK_SZ + x)*BLOCK_W;
        block->pos_y = (chunk->y*BLOCK_CHUNK_SZ + y)*BLOCK_H;
        block->x = (chunk->x*BLOCK_CHUNK_SZ + x);
        block->y = (chunk->y*BLOCK_CHUNK_SZ + y);
        block->size = BLOCK_W;
        block->life = 10;
        block->org_x = block->x;
        block->org_y = block->y;
        if(type == BL_ITEM) block->item_count = 1;
        else if(type == BL_ITEM_INF) block->item_count = 255;

        block->can_vanish = get_block_action_from_id(type, SIDE_TOP) == BA_VANISH
        || get_block_action_from_id(type, SIDE_RIGHT) == BA_VANISH
        || get_block_action_from_id(type, SIDE_BOTTOM) == BA_VANISH
        || get_block_action_from_id(type, SIDE_LEFT) == BA_VANISH;

        block->safe = !(block->can_vanish || block_is_fragile(type));

        block->vanish_len1 = 15;
        block->vanish_len2 = 25;
    }
}

Block* get_block_from_chunk(Chunk* chunk, int x, int y)
{
    return chunk->block[y][x];
}

void move_block(Level* lvl, int x1, int y1, int x2, int y2)
{
    if(x1 == x2 && y1 == y2) return;

    Block* src = get_block(lvl, x1, y1);
    int2 rot_pos = rotate_position(x2, y2, (4 - current_rotation) % 4, 1999);
    src->x = rot_pos.x;
    src->y = rot_pos.y;
    src->pos_x = src->x*BLOCK_W;
    src->pos_y = src->y*BLOCK_H;

    set_block_ptr(lvl, x2, y2, src);
    set_block_ptr(lvl, x1, y1, NULL);

}

bool push_block(Level* lvl, enum Direction dir, int x, int y)
{
    int add_x = 0;
    int add_y = 0;
    switch(dir)
    {
        case DIR_UP: add_y = -1; break;
        case DIR_DOWN: add_y = 1; break;
        case DIR_LEFT: add_x = -1; break;
        case DIR_RIGHT: add_x = 1; break;
    }

    Block* next = get_block(lvl, x+add_x, y+add_y);
    if(next == NULL)
    {
        move_block(lvl, x, y, x+add_x, y+add_y);
        return true;
    }
    else if(get_block_action(NULL, next, (dir+2)%4, current_rotation) == BA_PUSH)
    {
        if(push_block(lvl, dir, x+add_x, y+add_y))
        {
            move_block(lvl, x, y, x+add_x, y+add_y);
            return true;
        }
        return false;
    }
    return false;
}

bool is_player_at(Level* lvl, int x, int y)
{
    return (int)the_player.x / BLOCK_W == x && (int)the_player.y / BLOCK_H == y;
}

Level* load_pr2_level(const char* filename, bool load_art)
{
    Level* level = create_level();
    FILE* file = fopen(filename, "r");
    char c1, c2;
    int pos_x = 0;
    int pos_y = 0;
    int block_id;
    int x, y;
    char bgcol[7] = {0, 0, 0, 0, 0, 0, 0};
    bool fix = false;
    int debug;

    while(true)
    {
        char data[6];
        char c = fgetc(file);
        if(c == '&')
        {
            fscanf(file, "%5s", data);
            if(strcmp(data, "data=") == 0)
            {
                break;
            }
        }
    }

    while(true) //read background color
    {
        char c = fgetc(file);
        if(c == '`')
        {
            for(int i=0; i<7; i++)
            {
                bgcol[i] = fgetc(file);
                if(bgcol[i] == '`')
                {
                    bgcol[i] = 0;
                    int color;
                    sscanf(bgcol, "%x", &color);
                    level->bgcol.x = color >> 16;
                    level->bgcol.y = (color >> 8) & 0xff;
                    level->bgcol.z = color & 0xff;
                    goto break_loops;
                }
            }
        }
    }
    break_loops:

    fscanf(file, "%i %c %i %c %i %c", &x, &c1, &y, &c1, &block_id, &c2);
    pos_x += x; pos_y += y;
    level->camera_x = pos_x;
    level->camera_y = pos_y;
    level->start_pos[0] = (int2) {.x = pos_x, .y = pos_y};
    printf("START %i %i\n", pos_x, pos_y);

    fscanf(file, "%i %c %i %c %i %c", &x, &c1, &y, &c1, &block_id, &c2);
    pos_x += x; pos_y += y;

    fscanf(file, "%i %c %i %c %i %c", &x, &c1, &y, &c1, &block_id, &c2);
    pos_x += x; pos_y += y;

    fscanf(file, "%i %c %i %c %i %c", &x, &c1, &y, &c1, &block_id, &c2);
    pos_x += x; pos_y += y;

    while(true)
    {
        int ret = fscanf(file, "%i %c %i %c", &x, &c1, &y, &c2);
        if(ret == EOF) break;
        pos_x += x;
        pos_y += y;
        if(c2 == ';')
        {
            fscanf(file, "%i %c", &block_id, &c1);
            if(c1 == '`') break;
        }
        else if(c2 == '`')
        {
            break;
        }
        if( pos_x >= 2000 || pos_y >= 2000 || pos_x < 0 || pos_y < 0)
        {
            printf("Error: loading level: invalid block position %i %i\n", pos_x, pos_y);
        }
        else
        {
            set_block(level, pos_x, pos_y, convert_pr2old_id_to_pr4(block_id));
        }
    }

    //stamps
    char cc = fgetc(file);

    for(int counter = 0; counter < 3;)
    {
        if(fgetc(file) == '`') counter++;
    }

    if(load_art)
    {
        load_art_from_file(level, file, 0);
    }
    level->camera_x = level->start_pos[0].x*BLOCK_W;
    level->camera_y = level->start_pos[0].y*BLOCK_H;

    fclose(file);
    return level;
}

Block* find_teleport_of_color(int start_x, int start_y, int color)
{
    for(int i=0; i<the_level->chunk_list->elm_count; i++)
    {
        Chunk* chunk = exl_get_elm_ptr(the_level->chunk_list, i);
        for(int y=0; y<BLOCK_CHUNK_SZ; y++)
        {
            for(int x=0; x<BLOCK_CHUNK_SZ; x++)
            {
                Block* block = get_block_from_chunk(chunk, x, y);
                if(block != NULL && block->color == color && (block->x != start_x || block->y != start_y))
                {
                    return block;
                }
            }
        }
    }
    return NULL;
}

void render_and_handle_level_gui()
{ 
    static int current_music_id = -1;
    init_gui_rendering();
    render_and_handle_gui_list(gui_list_music);
    if(gui_list_music->selected != current_music_id)
    {
        play_music(gui_list_music->selected);
        current_music_id = gui_list_music->selected;
    }
}
