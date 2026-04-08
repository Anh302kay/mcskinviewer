#pragma once
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <3ds.h>
#include <citro3d.h>
#include <curl/curl.h>

#include "glbloader.hpp"

enum skinType{
    SKIN_CLASSIC = 0,
    SKIN_WIDE,
    SKIN_SLIM
};

enum {
    HEAD = BIT(0),
    TORSO = BIT(1),
    LARM = BIT(2),
    RARM = BIT(3),
    LLEG = BIT(4),
    RLEG = BIT(5),
    HAT = BIT(6),
    OTORSO = BIT(7),
    OLARM = BIT(8),
    ORARM = BIT(9),
    OLLEG = BIT(10),
    ORLEG = BIT(11),

    LAYER_ONE = HEAD | TORSO | LARM | RARM | LLEG | RLEG,
    LAYER_TWO = HAT | OTORSO | OLARM | ORARM | OLLEG | ORLEG,
    FULL_SKIN = LAYER_ONE | LAYER_TWO
};

enum {
    mesh_body = 0,
    mesh_head,
    mesn_larm,
    mesn_lleg,
    mesn_rarm,
    mesn_rleg
};

class Skin {
public:
    Skin();
    void cleanup();
    static void initModels();
    void download(const std::string& p_name);
    void use();
    void render();
    C3D_Tex skin;
    std::vector<u8> skinPNG;
    std::vector<u8> skinData;
    CURL* curl = NULL;
    skinType type;
    u16 visibility = FULL_SKIN;
    bool layerToggle[2] = {true, true};
    std::string name;
private:
    void renderSkin(std::array<Mesh, 6>& skin1);
    void renderSkin(std::array<Mesh, 6>& skin1, std::array<Mesh, 6>& skin2);
    static std::array<Mesh, 6> ogSkin;
    static std::array<Mesh, 6> slim1;
    static std::array<Mesh, 6> slim2;
    static std::array<Mesh, 6> wide1;
    static std::array<Mesh, 6> wide2;
};