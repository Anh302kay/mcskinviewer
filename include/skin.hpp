#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <3ds.h>
#include <citro3d.h>
#include <curl/curl.h>

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

class Skin {
public:
    Skin();
    ~Skin();
    void download(const std::string& p_name);
    void use();
    C3D_Tex skin;
    std::vector<u8> skinPNG;
    std::vector<u8> skinData;
    CURL* curl = NULL;
    skinType type;
    u16 visibility = FULL_SKIN;
    std::string name;
private:

};