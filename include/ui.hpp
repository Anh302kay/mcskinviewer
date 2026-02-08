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
    bool touched(const touchPosition& touchPos);
    void draw();

    enum : u8{
        img = 0,
        text
    };
};