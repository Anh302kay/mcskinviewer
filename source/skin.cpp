#include <iostream>
#include <string>
#include <vector>
#include <3ds.h>
#include <citro3d.h>
#include <curl/curl.h>
#include <external/lodepng.h>

#include "skin.hpp"

static size_t storeData(char* ptr, size_t size, size_t nmemb, void* vector) {
    std::vector<u8>* buffer = static_cast<std::vector<u8>*>(vector);
    buffer->insert(buffer->end(), (u8*)ptr, (u8*)ptr + (size * nmemb));
    // size_t written = fwrite(ptr, size, nmemb, stream);
    return size*nmemb;
}

static constexpr size_t posToTex(u16 index) 
{
    constexpr int width = 64;
    const int x = index % 64;
    const int y = index / 64;

    return ((((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
}

Skin::Skin() {
    curl = curl_easy_init();
    if(!curl) {
        std::cout << "could not init CURL";
        return;
    }
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, storeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &skinPNG);
    skinPNG.reserve(64*64);
    skinData.reserve(64*64);
    C3D_TexInit(&skin, 64, 64, GPU_RGBA8);
    C3D_TexSetFilter(&skin, GPU_NEAREST, GPU_NEAREST);
	C3D_TexSetWrap(&skin, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
}

Skin::~Skin() {
    curl_easy_cleanup(curl);
    curl = NULL;
    skin.height = 64;
    skin.size = 4000;
    C3D_TexDelete(&skin);
}

void Skin::use() {
    C3D_TexBind(0, &skin);
}

void Skin::download(const std::string& p_name) {
    const std::string URL = "https://minotar.net/skin/" + p_name;
    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cout << curl_easy_strerror(res);
        return;
    }
    name = p_name;
    unsigned int width, height;
    unsigned error = lodepng::decode(skinData, width, height, skinPNG);

    if(error) {
        std::cout << "png decoder error " << error << ": " << lodepng_error_text(error) << "\n";
        return;
    }

    u32* rgbaData = reinterpret_cast<u32*>(&skinData[0]);
    u32* texBuffer = reinterpret_cast<u32*>(skin.data);

    for(int i = 0; i < width * height; i++) 
        texBuffer[posToTex(i)] = std::byteswap(rgbaData[i]);
    
    skin.height = height;

    if(height == 32) {
        type = SKIN_CLASSIC;
        skin.size = 2000;
    }
    else {
        skin.size = 4000;
        type = (texBuffer[posToTex(19*64+50)] & 0xFF) ? SKIN_WIDE : SKIN_SLIM;
    }
}
