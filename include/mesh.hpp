#pragma once
#include <iostream>
#include <3ds.h>
#include <citro3d.h>

struct Mesh {
    Mesh();
    Mesh(const std::string& obj);
    ~Mesh();
    C3D_AttrInfo attrInfo;
    C3D_BufInfo bufInfo;
    void* meshData = nullptr;
    u32 meshSize = 0;
    u16* indices = nullptr;
    u32 indexCount = 0;
    void loadOBJ(const std::string& obj);
    void render();
};