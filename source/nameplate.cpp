#include <iostream>
#include <3ds.h>
#include <ranges>

#include "shader.hpp"
#include "nameplate.hpp"

static constexpr float vertices[] = {
    // positions         // uv
    -5.f, -0.5f, -1.f, 0.0f, 0.0f,   // bottom left
    +5.f, -0.5f, -1.f, 1.0f, 0.0f,   // bottom right
    +5.f,  +0.5f, -1.f, 1.0f, 1.0f,   // top right
    -5.f,  +0.5f, -1.f, 0.0f, 1.0f   // top left
};    

constexpr int meshSize = sizeof(vertices);

static constexpr u8 indicesOrder[] {
    0, 1, 2,
    0, 2, 3
};

constexpr int indicesSize = sizeof(indicesOrder);
constexpr int indicesCount = sizeof(indicesOrder);

static constexpr size_t posToTex(unsigned int x, unsigned int y) 
{
    constexpr int width = 128;
    return ((((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
}

Nameplate::Nameplate(const std::string& name) {

    meshData = linearAlloc(meshSize);
    indices = (u8*)linearAlloc(indicesSize);

    memcpy(meshData, vertices, sizeof(vertices));
    memcpy(indices, indicesOrder, sizeof(indicesOrder));

    AttrInfo_Init(&attrInfo);
    AttrInfo_AddLoader(&attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(&attrInfo, 1, GPU_FLOAT, 2);

	BufInfo_Init(&bufInfo);
	BufInfo_Add(&bufInfo, meshData, sizeof(float)*5, 2, 0x10);

    C3D_TexInit(&texture, 128,8, GPU_LA4);
    C3D_TexSetFilter(&texture, GPU_NEAREST, GPU_NEAREST);
	C3D_TexSetWrap(&texture, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);

    update(name);
}

Nameplate::~Nameplate() {
    if(meshData != nullptr) {
        linearFree(meshData);
        meshData = nullptr;
    }
    if(indices != nullptr) {
        linearFree(indices);
        indices = nullptr;
    }
    if(texture.data != nullptr) {
        C3D_TexDelete(&texture);
        texture.data = nullptr;
    }
}

void Nameplate::update(const std::string& name) {
    memset(texture.data, 0b00000000, texture.size);
    for(auto [index, text] : std::views::enumerate(name)) {
        const int value = text;
        int convertedIndex = 0;
        if      (value < 58) convertedIndex = value - 47;
        else if (value < 91) convertedIndex = value - 54;
        else if (value == 95) convertedIndex = 0;
        else if (value < 123) convertedIndex = value - 60;
        
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 6; x++)
            ((u8*)texture.data)[posToTex( x + index*6, y)] = textPixels[(convertedIndex*48)+y*6+x];
        }
    }
    int leftoverChars = 20 - name.size();
    offset = 0.5 * leftoverChars / 2;
}

void Nameplate::render() {
    C3D_TexBind(0, &texture);
    C3D_SetAttrInfo(&attrInfo);
    C3D_SetBufInfo(&bufInfo);
    C3D_DrawElements(GPU_TRIANGLES, indicesCount, C3D_UNSIGNED_BYTE, indices);
}
