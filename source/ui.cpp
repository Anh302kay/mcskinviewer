#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "ui.hpp"
#include "utils.hpp"

template <typename T1, typename T2>
static constexpr bool touchedBox(const touchPosition& touch, const T1 x, const T1 y, const T2 w, const T2 h) {
    return touch.px > x && touch.px < x + w && touch.py > y && touch.py < y + h;
}

static constexpr bool touchedBox(const touchPosition& touch, const rectI& rect) {
    return touch.px > rect.x && touch.px < rect.x + rect.w && touch.py > rect.y && touch.py < rect.y + rect.h;
}

const char* keyboardLUT = { "qwertyuiopasdfghjklzxcvbnm"
                            "QWERTYUIOPASDFGHJKLZXCVBNM"
                            "1234567890"                };

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
    textBuf = C2D_TextBufNew(21);
    font = C2D_FontLoad("romfs:/gfx/Minecraft.bcfnt");
}

UI::~UI() {
    C2D_TextBufDelete(textBuf);
    C2D_FontFree(font);
    for(auto& img : keyboard)
        img.tex = nullptr;
    C2D_SpriteSheetFree(keyboardSpritesheet);
}

void UI::update(Skin& skin) {
    const u32 kDown = hidKeysDown();
    const u32 kHeld = hidKeysHeld();
    hidTouchRead(&touch);
    switch (mode) {
    case MENU_KEYBOARD:
        if(kDown & KEY_TOUCH)
            keyboardInput(skin);
        break;
    default:
        break;
    }
}

void UI::draw() {
    C2D_DrawRectSolid(67, 216, 0, 188, 24, C2D_Color32(255,255,255,255));
    switch (mode) {
    case MENU_KEYBOARD:
        C2D_DrawImageAt(keyboard[capsLock], 0, 0, 0);
        C2D_DrawText(&text, C2D_WithColor | C2D_AtBaseline, 10, 20, 0, .8f, .8f, C2D_Color32(255,255,255,255));
        break;
    default:
        break;
    }
}

void UI::keyboardInput(Skin& skin) {
    static std::string cache;
    cache.reserve(20);
    u16 touchX = touch.px;
    u16 touchY = touch.py - 58;

    constexpr rectI del(272, 162, 24, 31);
    if(touchedBox(touch, del)) {
        cache.pop_back();
        C2D_TextBufClear(textBuf);
        C2D_TextFontParse(&text, font, textBuf, cache.c_str());
        C2D_TextOptimize(&text);
        return;
    }

    constexpr rectI capLock(3, 190, 42, 39);
    if(touchedBox(touch, capLock)) {
        capsLock = !capsLock;
        return;
    }

    constexpr rectI enter(288, 122, 29, 44);
    if(touchedBox(touch, enter)) {
        skin.download(cache);
        return;
    }

    if(cache.length() == 20)
        return;

    constexpr rectI underscore(272, 198, 24, 31);
    if(touchedBox(touch, underscore)) {
        cache += "_";
        C2D_TextBufClear(textBuf);
        C2D_TextFontParse(&text, font, textBuf, cache.c_str());
        C2D_TextOptimize(&text);
        return;
    }

    if(touch.py > 35 && touch.py < 57) {
        touchX -= 2;
        cache += keyboardLUT[touchX/30 + 52];
    }

    switch (touchY / 50) {
    case 0:
        touchX -= 7;
        if(touchX/30 < 10)
        cache += keyboardLUT[touchX/30 + capsLock * 26];
        break;
    case 1:
        touchX -= 15;
        if(touchX/30 < 9)
        cache += keyboardLUT[touchX/30 + capsLock * 26 + 10];
        break;
    case 2:
        touchX -= 55;
        if(touchX/30 < 7)
        cache += keyboardLUT[touchX/30 + capsLock * 26 + 19];
        break;
    default:
        break;
    }
    C2D_TextBufClear(textBuf);
    C2D_TextFontParse(&text, font, textBuf, cache.c_str());
    C2D_TextOptimize(&text);
}
