#pragma once
#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "utils.hpp"

struct button {
    union {
        C2D_Text text;
        C2D_Image img;
    };
    v2f pos;
    v2f dim;
    v2f scale;
    v2f offset;
    float outline;
    u32 colour;
    u8 mode;
    button(v2f p_pos, v2f p_dim, u32 p_colour = C2D_Color32(255,255,255,255), float outline = 1.f, v2f scale = v2f(1.f), v2f offset = v2f(0.f));
    bool touched(const touchPosition& touch);
    void draw();

    enum : u8{
        NONE = 0,
        IMAGE,
        TEXT,
    };
};