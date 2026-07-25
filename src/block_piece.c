#include "block_piece.h"
#include "tools.h"
#include "block.h"
#include "render.h"
#include "graph.h"
#include "level.h"

Pointer_list* block_pieces = NULL;

void init_block_pieces()
{
    block_pieces = ptrl_create_list(10, 2);
}

void add_block_piece(Block_piece* bp)
{
    ptrl_add_pointer(block_pieces, bp, 1);
}

Block_piece* create_block_piece(Gl_texture* tex, float param2, float param3, float param4, float param5, float param6, float param7, float param8, float param9)
{
    Block_piece* ret = malloc(sizeof(Block_piece));
    ret->x = param8;
    ret->y = param9;
    ret->gravity = param2;
    ret->friction = param3;
    ret->name_3 = param4;
    ret->rotation = random_float() * 360;
    ret->vel_x = random_float() * param5 * 2 - param5;
    ret->vel_y = random_float() * param6 * 2 - param6;
    ret->var_372 = random_float() * param7 * 2 - param7;   
    ret->alpha = 1; 
    ret->tex = tex;
    return ret;
}

void block_piece_remove(Block_piece* bp)
{ 
    for(int i=0; i<block_pieces->elm_counter; i++)
    {
        if(ptrl_get_pointer(block_pieces, i) == bp)
        {
            ptrl_remove_elm(block_pieces, i);
            break;
        }
    }
}

void block_piece_update(Block_piece* bp)
{    
    if(bp == NULL) return;   

    bp->vel_x *= bp->friction;
    bp->vel_y *= bp->friction;
    bp->var_372 *= bp->friction;
    bp->vel_y += bp->gravity;
    bp->x += bp->vel_x;
    bp->y += bp->vel_y;
    bp->rotation += bp->var_372;
    bp->alpha -= bp->name_3;
    if(bp->alpha <= 0)
    {
        //TODO remove
        block_piece_remove(bp);
    }
}

void update_block_pieces()
{
    for(int i=0; i<block_pieces->elm_counter; i++)
    {
        Block_piece* bp = ptrl_get_pointer(block_pieces, i);
        block_piece_update(bp);
    }
}

void render_block_piece(Block_piece* bp)
{
    if(bp == NULL) return;
    glColor4f(1, 1, 1, bp->alpha);
    float frag_x = random_float()*0.5;
    float frag_y = random_float()*0.5;
    render_texture_frag_rot(bp->tex, frag_x, frag_y, frag_x+0.5, frag_y+0.5, bp->x-the_level->camera_x, bp->y-the_level->camera_y, bp->rotation);
    glColor4f(1, 1, 1, 1);
}

void render_block_pieces()
{
    for(int i=0; i<block_pieces->elm_counter; i++)
    {        
        Block_piece* bp = ptrl_get_pointer(block_pieces, i);       
        render_block_piece(bp);        
    }
}