#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <ranges>

#include "ui.hpp"
#include "utils.hpp"

template <typename T1, typename T2>
static constexpr bool touchedBox(const touchPosition& touch, const T1 x, const T1 y, const T2 w, const T2 h) {
    return touch.px > x && touch.px < x + w && touch.py > y && touch.py < y + h;
}

static constexpr bool touchedBox(const touchPosition& touch, const rectI& rect) {
    return touch.px > rect.x && touch.px < rect.x + rect.w && touch.py > rect.y && touch.py < rect.y + rect.h;
}

static constexpr void setSkinControls(button visButtons[NUMVISBUTTONS], const int xPos, const int yPos, float ratio) {
    for(int i = 0; i < 2; i++) {
        constexpr float width = 30.f;
        visButtons[VIS_HEAD + i * 6] = button(v2f(xPos    + i*160.f, yPos), v2f(width), C2D_Color32(255,255,255,255), 2.f);
        visButtons[VIS_TORSO + i * 6] = button(v2f(xPos   + i*160.f, yPos+width+2), v2f(width, width*1.5f), C2D_Color32(255,255,255,255), 2.f);
        visButtons[VIS_LARM + i * 6] = button(v2f(xPos-width/ratio-2 + i*160.f, yPos+width+2), v2f(width/ratio, width*1.5f), C2D_Color32(255,255,255,255), 2.f);
        visButtons[VIS_RARM + i * 6] = button(v2f(xPos+width+2 + i*160.f, yPos+width+2), v2f(width/ratio, width*1.5f), C2D_Color32(255,255,255,255), 2.f);
        visButtons[VIS_LLEG + i * 6] = button(v2f(xPos    + i*160.f, yPos+width*2.5+4), v2f(width/2-1 , width*1.5f), C2D_Color32(255,255,255,255), 2.f);
        visButtons[VIS_RLEG + i * 6] = button(v2f(xPos+width/2+1 + i*160.f, yPos+width*2.5+4), v2f(width/2-1 , width*1.5f), C2D_Color32(255,255,255,255), 2.f);
    }
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

constexpr const char* uiTextLookup[] {
    "First Layer",
    "Second Layer",
    "Press A to enable freelook"
};

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

    cameraButtons[CAMERA_TRACKPAD] = button(v2f(20.f, 10.f), v2f(280.f, 193.f));

    setSkinControls(visButtons, 65, 30, 2);
    // visButtons[VIS_LAYER1] = button(v2f(70, 160), v2f(20,20));
    // visButtons[VIS_LAYER2] = button(v2f(230, 160), v2f(20,20));
    visButtons[VIS_LAYER1] = button(C2D_SpriteSheetGetImage(iconSpritesheet, 4), v2f(58.5, 160), v2f(45,23), 0);
    visButtons[VIS_LAYER2] = button(C2D_SpriteSheetGetImage(iconSpritesheet, 4), v2f(218.5, 160), v2f(45,23), 0);
    for(auto [i, text] : std::views::enumerate(uiText))
        C2D_TextFontParse(&text, font, skinTextbuf, uiTextLookup[i]);
    

}

UI::~UI() {
    C2D_TextBufDelete(skinTextbuf);
    C2D_TextBufDelete(keyTextBuf);
    C2D_FontFree(font);
    keyTextBuf = nullptr;
    skinTextbuf = nullptr;
    font = nullptr;
    C2D_SpriteSheetFree(iconSpritesheet);
    C2D_SpriteSheetFree(keyboardSpritesheet);
    for(auto& img : keyboard)
        img.tex = nullptr;
}

void UI::update(Skin& skin, Transform& skinTransform, Camera& camera) {
    const u32 kDown = hidKeysDown();
    const u32 kHeld = hidKeysHeld();
    hidTouchRead(&touch);
    // C2D_TextBufClear(skinTextbuf);
    // const std::string temp = std::format("{}, {}, {}", camera.position.z+1, camera.getDelta().x, camera.getDelta().y);
    // C2D_TextParse(&debugText, skinTextbuf, temp.c_str());

    if(kDown & KEY_A) {
        camera.viewLock = !camera.viewLock;
        camera.xLock = camera.viewLock;
        if(camera.viewLock) {
            camera.position.y = 0.f;
            float angle = atan2f(camera.position.z+1.f,camera.position.x) * ( 180.f / M_PI);
            if(angle < 0) angle += 360.f;
            camera.resetAngle();
            camera.rotateCamera({angle-90.f, 0.f});
        }
    }


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
    case MENU_VISIBILITY:
        visUpdate(skin);
        break;
    case MENU_CAMERA:
        cameraUpdate(camera, skinTransform);
        break;
    default:
        break;
    }
}

void UI::draw(Skin& skin) {
    constexpr u32 white = C2D_Color32(255,255,255,255);
    constexpr u32 grey = C2D_Color32(128,128,128,255);
    C2D_DrawRectSolid(60, 213, 0, 202, 27, C2D_Color32(0,0,0,255));
    // C2D_DrawText(&debugText, 0, 50, 30, 0, .5f, .5f);
    for(button& b : menuButtons)
        b.draw();

    switch (mode) {
    case MENU_KEYBOARD:
        C2D_DrawImageAt(keyboard[capsLock], 0, 0, 0);
        C2D_DrawText(&skinText, C2D_WithColor | C2D_AtBaseline, 10, 25, 0, .8f, .8f, white);
        break;
    case MENU_VISIBILITY:
        for(button& b : visButtons)
            b.draw();
        switch(skin.type) {
            case SKIN_SLIM:
            case SKIN_WIDE:
                C2D_DrawText(&uiText[0], C2D_WithColor,  40, 10, 0, .5f, .5f, skin.layerToggle[0] ? white : grey);
                C2D_DrawText(&uiText[1], C2D_WithColor, 195, 10, 0, .5f, .5f, skin.layerToggle[1] ? white : grey);
                C2D_DrawLine(160, 10, white, 160, 160, white, 2, 0);
                break;
            case SKIN_CLASSIC:
                C2D_DrawText(&uiText[0], C2D_WithColor, 120, 10, 0, .5f, .5f, white);
                break;
        }

        break;
    case MENU_CAMERA:
        for(button& b : cameraButtons)
            b.draw();

        C2D_DrawText(&uiText[UI_FREELOOK], C2D_WithColor | C2D_AlignCenter, 160, 106.5, 0, 0.5f, 0.5f, white);
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
        switch(skin.type) {
            case SKIN_CLASSIC:
                setSkinControls(visButtons, 145, 30, 2);
                visButtons[VIS_LAYER1].pos = v2f(138.5, 160);
                visButtons[VIS_LAYER2].pos = v2f(230, 240);
                for(int i = 0; i < 6; i++)
                    visButtons[i+6].pos.y = 240;

            break;
            case SKIN_WIDE:
                setSkinControls(visButtons, 65, 30, 2);
                visButtons[VIS_LAYER1].pos = v2f(58.5, 160);
                visButtons[VIS_LAYER2].pos = v2f(218.5, 160);
                break;
            case SKIN_SLIM:
                setSkinControls(visButtons, 65, 30, 2.5f);
                visButtons[VIS_LAYER1].pos = v2f(58.5, 160);
                visButtons[VIS_LAYER2].pos = v2f(218.5, 160);
                break;
        }
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

    if(!(kDown & KEY_TOUCH)) return;

    // if(cameraButtons[CAMERA_FREELOOK].touched(touch)) {
    //     camera.viewLock = !camera.viewLock;
    //     camera.xLock = camera.viewLock;
    //     if(camera.viewLock) {
    //         camera.position.y = 0.f;
    //         float angle = atan2f(camera.position.z+1.f,camera.position.x) * ( 180.f / M_PI);
    //         if(angle < 0) angle += 360.f;
    //         camera.resetAngle();
    //         camera.rotateCamera({angle-90.f, 0.f});
    //     }
    // }
}

void UI::visUpdate(Skin& skin) {
    const u32 kDown = hidKeysDown();
    constexpr u32 white = C2D_Color32(255,255,255,255);
    constexpr u32 grey = C2D_Color32(128,128,128,255);
    constexpr u32 darkGrey = C2D_Color32(96,96,96,255);

    if(!skin.layerToggle[0]) {
        for(int i = 0; i < 6; i++) {
            if(visButtons[i+6].touched(touch) && (kDown & KEY_TOUCH)) {
                skin.visibility ^= 1 << (i+6);
                bool bit = (skin.visibility >> (i+6)) & 1;
                visButtons[i+6].colour = bit ? white : grey;
            }
        }
    }
    if(!skin.layerToggle[1]) {
        for(int i = 0; i < 6; i++) {
            if(visButtons[i].touched(touch) && (kDown & KEY_TOUCH)) {
                skin.visibility ^= 1 << i;
                bool bit = (skin.visibility >> i) & 1;
                visButtons[i].colour = bit ? white : grey;
            }
        }
    }
    if(skin.layerToggle[0] && skin.layerToggle[1]) {
        for(int i = 0; i < SKINGRAPHICS; i++) {
            if(visButtons[i].touched(touch) && (kDown & KEY_TOUCH)) {
                skin.visibility ^= 1 << i;
                bool bit = (skin.visibility >> i) & 1;
                visButtons[i].colour = bit ? white : grey;
            }
        }
    }


    if(kDown & KEY_TOUCH) {
        if(visButtons[VIS_LAYER1].touched(touch)) {
            skin.layerToggle[0] = !skin.layerToggle[0];
            for(int i = 0; i < 6; i++) {
                bool bit = (skin.visibility >> i) & 1;
                visButtons[i].colour = skin.layerToggle[0] ? bit ? white : grey : darkGrey;
            }
            visButtons[VIS_LAYER1].img = C2D_SpriteSheetGetImage(iconSpritesheet, 3+skin.layerToggle[0]);
        }

        if(visButtons[VIS_LAYER2].touched(touch)) {
            skin.layerToggle[1] = !skin.layerToggle[1];
            for(int i = 0; i < 6; i++) {
                bool bit = (skin.visibility >> (i+6)) & 1;
                visButtons[i+6].colour = skin.layerToggle[1] ? bit ? white : grey : darkGrey;
            }
            visButtons[VIS_LAYER2].img = C2D_SpriteSheetGetImage(iconSpritesheet, 3+skin.layerToggle[1]);
        }
    }
}

