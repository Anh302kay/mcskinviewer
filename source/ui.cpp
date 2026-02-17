#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "ui.hpp"
#include "utils.hpp"

const char* keyboardLUT = {"qwertyuiopasdfghjklzxcvbnm"
                           "QWERTYUIOPASDFGHJKLZXCVBNM"};

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
        case IMAGE:
            break;

        case TEXT:
            break;
        
        case NONE:
        default:
            break;
    }
}

UI::UI() {
    keyboardSpritesheet = C2D_SpriteSheetLoad("romfs:/gfx/keyboard.t3x");
    keyboard[0] = C2D_SpriteSheetGetImage(keyboardSpritesheet, 0);
    keyboard[1] = C2D_SpriteSheetGetImage(keyboardSpritesheet, 1);
}

UI::~UI() {
    C2D_SpriteSheetFree(keyboardSpritesheet);
    for(auto& img : keyboard)
        img.tex = nullptr;
}

void UI::update(touchPosition touch, auto& skin) {
    switch (mode) {
    case MENU_KEYBOARD:
        keyboardInput(touch);
        break;
    default:
        break;
    }
}

void UI::draw() {
    switch (mode) {
    case MENU_KEYBOARD:
        C2D_DrawImageAt(keyboard[capsLock], 0, 0, 0);
        break;
    default:
        break;
    }
}

void UI::keyboardInput(touchPosition touch) {
    static std::string cache;
    cache.reserve(20);
    touch.py -= 58;
    switch (touch.py / 50) {
    case 0:
        touch.px -= 7;
        cache += keyboardLUT[touch.px/60 + capsLock * 26];
        break;
    case 1:
        touch.px -= 15;
        cache += keyboardLUT[touch.px/60 + capsLock * 26 + 10];
        break;
    case 2:
        touch.px -= 55;
        cache += keyboardLUT[touch.px/60 + capsLock * 26 + 19];
        break;
    default:
        break;
    }
}
