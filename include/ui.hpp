#pragma once
#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "skin.hpp"
#include "nameplate.hpp"
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
    MENU_KEYBOARD,
    MENU_VISIBILITY,
    MENU_CAMERA,
    MENU_TRACKPAD,
    NUMMENUS = 4
};

enum {
    CAMERA_TRACKPAD,
    NUMCAMERABUTTONS = 1
};

enum {
    VIS_HEAD,
    VIS_TORSO,
    VIS_RARM,
    VIS_LARM,
    VIS_RLEG,
    VIS_LLEG,
    VIS_HAT,
    VIS_OTORSO,
    VIS_ORARM,
    VIS_OLARM,
    VIS_ORLEG,
    VIS_OLLEG,
    VIS_LAYER1,
    VIS_LAYER2,
    NUMVISBUTTONS = 14,
    SKINGRAPHICS=12
};

enum {
    UI_FIRSTLAYER=0,
    UI_SECONDLAYER,
    UI_FREELOOK,
    NUMUITEXT=3
};

class UI {
public:
    UI();
    ~UI();
    void update(Skin& skin, Nameplate& nameplate, Transform& skinTransform, Camera& camera);
    void draw(Skin& skin);
private:
    void keyboardInput(Skin& skin, Nameplate& nameplate);
    void cameraUpdate(Camera& camera, Transform& skinTransform);
    void visUpdate(Skin& skin);
    u8 mode = MENU_KEYBOARD;
    bool capsLock = false;
    button menuButtons[NUMMENUS];
    button cameraButtons[NUMCAMERABUTTONS];
    button visButtons[NUMVISBUTTONS];
    C2D_SpriteSheet keyboardSpritesheet;
    C2D_SpriteSheet iconSpritesheet;
    C2D_Image keyboard[2];
    C2D_TextBuf skinTextbuf;
    C2D_TextBuf keyTextBuf;
    C2D_Font font;
    C2D_Text uiText[NUMUITEXT];
    C2D_Text skinText;
    C2D_Text debugText;
    touchPosition touch;
};