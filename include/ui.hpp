#pragma once
#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "skin.hpp"
#include "camera.hpp"
#include "utils.hpp"
struct button {
    button() {}
    button(v2f p_pos, v2f p_dim, u32 p_colour = C2D_Color32(255,255,255,255), float p_outline = 1.f, v2f p_scale = v2f(1.f), v2f p_offset = v2f(0.f));
    button(C2D_Image p_img, v2f p_pos, v2f p_dim, u32 p_colour = C2D_Color32(255,255,255,255), float p_outline = 1.f, v2f p_scale = v2f(1.f), v2f p_offset = v2f(0.f));
    bool touched(const touchPosition& touch);
    void draw();

    union {
        C2D_Text text;
        C2D_Image img;
    };
    v2f pos = v2f(0);
    v2f dim = v2f(0);
    v2f scale = v2f(0);
    v2f offset = v2f(0);
    float outline = 1.f;
    u32 colour = 0;
    u8 mode = NONE;

    enum : u8{
        NONE = 0,
        IMAGE,
        TEXT,
    };
};

enum {
    MENU_CAMERA,
    MENU_VISIBILITY,
    MENU_KEYBOARD,
    MENU_TRACKPAD
};

enum {
    BUTTON_VISIBILITY,
    BUTTON_KEYBOARD,
    NUMBUTTONS = 2
};

class UI {
public:
    UI();
    ~UI();
    void update(Skin& skin, Transform& skinTransform, Camera& camera);
    void draw();
private:
    void keyboardInput(Skin& skin);
    u8 mode = MENU_KEYBOARD;
    bool capsLock = false;
    button buttons;
    C2D_SpriteSheet keyboardSpritesheet;
    C2D_SpriteSheet iconSpritesheet;
    C2D_Image keyboard[NUMBUTTONS];
    C2D_TextBuf skinTextBuf;
    C2D_Font font;
    C2D_Text skinText;
    touchPosition touch;
};