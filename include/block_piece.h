#pragma once

#include <stdlib.h>
#include "pointer_list.h"
#include "gl_texture.h"

typedef struct Block_piece
{
    Gl_texture* tex;
    float x;
    float y;
    float vel_x;
    float vel_y;
    float var_372;
    float gravity;
    float friction;
    float name_3;
    float rotation;
    float alpha;
} Block_piece;

extern Pointer_list* block_pieces;

void init_block_pieces();

void add_block_piece(Block_piece *bp);

Block_piece *create_block_piece(Gl_texture* tex, float param2, float param3, float param4, float param5, float param6, float param7, float param8, float param9);
void block_piece_update(Block_piece *bp);

void update_block_pieces();

void render_block_pieces();
