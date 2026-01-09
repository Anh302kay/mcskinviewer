#pragma once
#include <iostream>
#include <3ds.h>
#include <citro3d.h>

struct Mesh {
    Mesh(const std::string& obj);
    ~Mesh();
    C3D_AttrInfo attrInfo;
    C3D_BufInfo bufInfo;
    void* meshData;
    u32 meshSize;
    void loadOBJ(const std::string& obj);
    void render();
};