#pragma once
#include <iostream>
#include <3ds.h>
#include <ranges>

#include "shader.hpp"
#include "characters.hpp"

class Nameplate {
public:
    Nameplate(const std::string& name);
    ~Nameplate();
    void update(const std::string& name);
    void render();
    float offset;
private:
    C3D_Tex texture;
    C3D_AttrInfo attrInfo;
    C3D_BufInfo bufInfo;
    void* meshData = nullptr;
    u8* indices = nullptr;
};
