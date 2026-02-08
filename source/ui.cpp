#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "ui.hpp"
#include "utils.hpp"

button::button(v2f p_pos, v2f p_dim, u32 p_colour, float p_outline, v2f p_scale, v2f p_offset) 
    :pos(p_pos), dim(p_dim), scale(p_scale), offset(p_offset), outline(p_outline), colour(p_colour)
{

}


bool button::touched(const touchPosition& touch) {
    return (touch.px > pos.x && touch.py > pos.y && touch.px <  pos.x + dim.x && touch.py < pos.y + dim.y);
}

void button::draw() {
    if(outline == 0) return;

    constexpr u32 background = C2D_Color32(255, 199, 199, 255);

    C2D_DrawRectSolid(pos.x, pos.y, 0, dim.x, dim.y, colour);
    C2D_DrawRectSolid(pos.x+outline, pos.y+outline, 0, dim.x-outline*2, dim.y-outline*2, background);

    switch(mode) {
        default:
        break;
    }
}
