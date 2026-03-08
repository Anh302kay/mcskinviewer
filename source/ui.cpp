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
{}

button::button(C2D_Image p_img, v2f p_pos, v2f p_dim, u32 p_colour, float p_outline, v2f p_scale, v2f p_offset) 
    :img(p_img), pos(p_pos), dim(p_dim), scale(p_scale), offset(p_offset), outline(p_outline), colour(p_colour), mode(IMAGE)
{}

bool button::touched(const touchPosition& touch) {
    return (touch.px > pos.x && touch.py > pos.y && touch.px <  pos.x + dim.x && touch.py < pos.y + dim.y);
}

void button::draw() {
    if(outline != 0) {
    constexpr u32 background = C2D_Color32(255, 199, 199, 255);
    C2D_DrawRectSolid(pos.x, pos.y, 0, dim.x, dim.y, colour);
    C2D_DrawRectSolid(pos.x+outline, pos.y+outline, 0, dim.x-outline*2, dim.y-outline*2, background);
    }

    switch(mode) {
        case IMAGE:
            C2D_DrawImageAt(img, pos.x+offset.x, pos.y+offset.y, 0);
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
    iconSpritesheet = C2D_SpriteSheetLoad("romfs:/gfx/icons.t3x");
    keyboard[0] = C2D_SpriteSheetGetImage(keyboardSpritesheet, 0);
    keyboard[1] = C2D_SpriteSheetGetImage(keyboardSpritesheet, 1);
    keyTextBuf = C2D_TextBufNew(21);
    skinTextbuf = C2D_TextBufNew(128);
    font = C2D_FontLoad("romfs:/gfx/Minecraft.bcfnt");

    menuButtons[MENU_KEYBOARD] = button(C2D_SpriteSheetGetImage(iconSpritesheet, 1), {65, 215}, {27,27}, C2D_Color32(255,255,255,255), 0.f, v2f(1.f), v2f(1.f));

    menuButtons[MENU_VISIBILITY] = button(C2D_SpriteSheetGetImage(iconSpritesheet, 0), {95, 215}, {27,27}, C2D_Color32(255,255,255,255), 0.f, v2f(1.f), v2f(1.f));

    menuButtons[MENU_CAMERA] = button({125, 215}, {27,27}, C2D_Color32(255,255,255,255), 20.f, v2f(1.f), v2f(1.f));

    cameraButtons[CAMERA_FREELOOK] = button(v2f(10.f), v2f(20.f));;
    cameraButtons[CAMERA_TRACKPAD] = button(v2f(20, 100.f), v2f(280.f, 120.f));
    cameraButtons[CAMERA_SCROLL] = button(v2f(50.f), v2f(20.f));;

}

UI::~UI() {
    C2D_TextBufDelete(skinTextbuf);
    C2D_TextBufDelete(keyTextBuf);
    C2D_FontFree(font);
    keyTextBuf = nullptr;
    skinTextbuf = nullptr;
    font = nullptr;
    for(auto& img : keyboard)
        img.tex = nullptr;
    C2D_SpriteSheetFree(iconSpritesheet);
    C2D_SpriteSheetFree(keyboardSpritesheet);
}

void UI::update(Skin& skin, Transform& skinTransform, Camera& camera) {
    const u32 kDown = hidKeysDown();
    const u32 kHeld = hidKeysHeld();
    hidTouchRead(&touch);
    C2D_TextBufClear(skinTextbuf);
    const std::string temp = std::format("{}, {}, {}", camera.yaw, camera.getDelta().x, camera.getDelta().y);
    C2D_TextParse(&debugText, skinTextbuf, temp.c_str());

    for(int i = 0; i < NUMMENUS; i++) {
        if(menuButtons[i].touched(touch) && kDown & KEY_TOUCH) {
            mode = i;
        }
    }

    switch (mode) {
    case MENU_KEYBOARD:
        if(kDown & KEY_TOUCH)
            keyboardInput(skin);
        break;
    case MENU_CAMERA:
        cameraUpdate(camera, skinTransform);
        break;
    default:
        break;
    }
}

void UI::draw() {
    C2D_DrawRectSolid(60, 213, 0, 202, 27, C2D_Color32(0,0,0,255));
    C2D_DrawText(&debugText, 0, 50, 30, 0, .5f, .5f);
    for(button& b : menuButtons)
        b.draw();

    switch (mode) {
    case MENU_KEYBOARD:
        C2D_DrawImageAt(keyboard[capsLock], 0, 0, 0);
        C2D_DrawText(&skinText, C2D_WithColor | C2D_AtBaseline, 10, 25, 0, .8f, .8f, C2D_Color32(255,255,255,255));
        break;
    case MENU_CAMERA:
        for(button& b : cameraButtons)
            b.draw();
        break;
    default:
        break;
    }
}

void UI::keyboardInput(Skin& skin) {
    static std::string cache;
    cache.reserve(20);
    constexpr u16 keyboardOffset = 55;
    u16 touchX = touch.px;
    u16 touchY = touch.py - keyboardOffset;
    const u32 kDown = hidKeysDown();

    constexpr rectI del(272, 162, 24, 31);
    if((touchedBox(touch, del) || kDown & KEY_B) && !cache.empty()) {
        cache.pop_back();
        C2D_TextBufClear(keyTextBuf);
        C2D_TextFontParse(&skinText, font, keyTextBuf, cache.c_str());
        C2D_TextOptimize(&skinText);
        return;
    }

    constexpr rectI capLock(3, 190, 42, 39);
    if(touchedBox(touch, capLock)) {
        capsLock = !capsLock;
        return;
    }

    constexpr rectI enter(288, 122, 29, 44);
    if(touchedBox(touch, enter) || (kDown & KEY_A)) {
        skin.download(cache);
        return;
    }

    if(cache.length() == 20)
        return;

    constexpr rectI underscore(272, 198, 24, 31);
    if(touchedBox(touch, underscore)) {
        cache += "_";
        C2D_TextBufClear(keyTextBuf);
        C2D_TextFontParse(&skinText, font, keyTextBuf, cache.c_str());
        C2D_TextOptimize(&skinText);
        return;
    }

    if(touch.py > keyboardOffset - 23 && touch.py < 57) {
        touchX -= 2;
        cache += keyboardLUT[touchX/30 + 52];
        C2D_TextBufClear(keyTextBuf);
        C2D_TextFontParse(&skinText, font, keyTextBuf, cache.c_str());
        C2D_TextOptimize(&skinText);
        return;
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
    C2D_TextBufClear(keyTextBuf);
    C2D_TextFontParse(&skinText, font, keyTextBuf, cache.c_str());
    C2D_TextOptimize(&skinText);
}

void UI::cameraUpdate(Camera& camera, Transform& skinTransform) {
    const u32 kDown = hidKeysDown();
    const u32 kHeld = hidKeysHeld();
    const v2f touchDelta = camera.getDelta();

    if(cameraButtons[CAMERA_SCROLL].touched(touch) && (kHeld & KEY_TOUCH)) {
        camera.rotateCamera({0, touchDelta.y*30});
    }

    if(!(kDown & KEY_TOUCH)) return;

    if(cameraButtons[CAMERA_FREELOOK].touched(touch)) {
        camera.viewLock = !camera.viewLock;
        camera.xLock = camera.viewLock;
        if(camera.viewLock) {
            camera.position.y = 0.f;
            camera.resetAngle();
            // camera.position = FVec3_New(0.f, 0.f, 1.f);
            float angle = atan2f(camera.position.z+1.f,camera.position.x) * ( 180.f / M_PI);
            if(angle < 0) angle += 360.f;
            camera.rotateCamera({angle-90.f, 0.f});
        }

    }


    
        // if(cameraButtons[i].touched(touch) && kDown & KEY_TOUCH) {
        //     switch(i) {
        //     case CAMERA_FREELOOK:
        //         camera.viewLock = !camera.viewLock;
        //         camera.xLock = !camera.xLock;
        //         break;
        //     case CAMERA_TRACKPAD:
        //         break;
        //     default:
        //         break;
        //     }
        // }
}