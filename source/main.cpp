#include <iostream>
#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <tex3ds.h>
#include <vector>
#include <ranges>
#include <bit>
#include <malloc.h>
#include <curl/curl.h>
#include <external/lodepng.h>
#include <memory>

#include "utils.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "objloader.hpp"
#include "mesh.hpp"
#include "skin.hpp"
#include "glbloader.hpp"
#include "ui.hpp"

#include "characters.hpp"
#include "colour_shbin.h"
#include "billboard_shbin.h"
#include "tex_shbin.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

float vertices2[] = {
    // positions         // colors
    -0.5f, -0.5f, -1.f, 0.0f, 0.0f,   // bottom left
    +0.5f, -0.5f, -1.f, 1.0f, 0.0f,   // bottom right
    +0.5f,  +0.5f, -1.f, 1.0f, 1.0f,   // top right
    -0.5f,  +0.5f, -1.f, 0.0f, 1.0f   // top left
};    

u8 ebo[] {
    0, 1, 2,
    0, 2, 3
};

static bool loadTex(const std::string& path, C3D_Tex* tex, C3D_TexCube* cube)
{
	FILE* fp = fopen(path.c_str(), "rb");

	Tex3DS_Texture t3x = Tex3DS_TextureImportStdio(fp, tex, cube, false);
	if (!t3x) {
	    fclose(fp);
		return false;
    }

	// Delete the t3x object since we don't need it
	Tex3DS_TextureFree(t3x);
	fclose(fp);
	return true;
}

static constexpr size_t posToTex(u16 index, int width) 
{
    // constexpr int width = 8;
    const int x = index % width;
    const int y = index / width;

    return ((((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
}

static inline size_t posToTex(unsigned int x, unsigned int y, int width) 
{
    return ((((y >> 3) * (width >> 3) + (x >> 3)) << 6) + ((x & 1) | ((y & 1) << 1) | ((x & 2) << 1) | ((y & 2) << 2) | ((x & 4) << 2) | ((y & 4) << 3)));
}

int main(int argc, char* argv[]) {
	romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    // consoleInit(GFX_BOTTOM, NULL);

    u32 *soc_sharedmem;
    soc_sharedmem = (u32*)memalign(0x1000, 0x100000);
    socInit(soc_sharedmem, 0x100000);

    C3D_RenderTarget* top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	C3D_RenderTarget* bottom = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    C3D_TexEnv env;
	C3D_TexEnvInit(&env);
	C3D_TexEnvSrc(&env, C3D_Both, GPU_TEXTURE0);
	C3D_TexEnvFunc(&env, C3D_Both, GPU_REPLACE);
	C3D_SetTexEnv(0, &env);

    Shader shader((u32*)tex_shbin, tex_shbin_size);
    shader.use();

    Camera camera;
	camera.viewLock = true;
    camera.xLock = true;

	auto ui = std::make_unique<UI>();

    Skin skin;
    skin.initModels();
    skin.download("Bob_Gangster");
    Transform skinTransform (v3f(0,-1.25, -1), v3f(0.f), v3f(1.f));

	circlePosition cPad;

    void* vbo = linearAlloc(sizeof(vertices2));
    memcpy(vbo, vertices2, sizeof(vertices2));

    void* eboBuffer = linearAlloc(sizeof(ebo));
    memcpy(eboBuffer, ebo, sizeof(ebo));

    C3D_Tex steve;
    // loadTex("romfs:/gfx/steve.t3x", &steve, nullptr);
    C3D_TexInit(&steve, 256,8, GPU_LA4);
    memset(steve.data, 0b00001111, steve.size);
    for(auto [index, text] : std::views::enumerate(skin.name)) {
        const int value = text;
        int convertedIndex;
        if(value < 58)
            convertedIndex = value - 46;
        else if (value < 91)
            convertedIndex = value - 54;
        else if (value == 95)
            convertedIndex = 0;
        else if (value < 123) {
            convertedIndex = value - 60;
        }
        for(int y = 0; y < 8; y++) {
            for(int x = 0; x < 8; x++)
            ((u8*)steve.data)[posToTex( x + index*8, y, steve.width)] = textPixels[(convertedIndex*64)+y*8+x];
        }
    }
    // for(int i = 0; i < steve.size; i++) {
    //     ((u8*)steve.data)[posToTex(i, 8)] = textPixels[i+128];
    // }
    C3D_TexSetFilter(&steve, GPU_NEAREST, GPU_NEAREST);
	C3D_TexSetWrap(&steve, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);

    C3D_AttrInfo attr;
    AttrInfo_Init(&attr);
    AttrInfo_AddLoader(&attr, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(&attr, 1, GPU_FLOAT, 2);

    C3D_BufInfo bufInfo;
	BufInfo_Init(&bufInfo);
	BufInfo_Add(&bufInfo, vbo, sizeof(float)*5, 2, 0x10);

    Shader billboard((u32*)billboard_shbin, billboard_shbin_size);

    while (aptMainLoop())
    {
        hidScanInput();
		hidCircleRead(&cPad);

        const u32 kDown = hidKeysDown();
        if(kDown & KEY_START)
            break;

        camera.update();
		ui->update(skin, skinTransform, camera);
        
        if(camera.viewLock && camera.deadZoneX())
		    skinTransform.rotation.y -= cPad.dx / 96.f;

        const C3D_Mtx lookAt = camera.getLookAt();
        const C3D_Mtx projection = camera.projection;

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(top, C3D_CLEAR_ALL, C2D_Color32(1, 128, 199, 255), 0);
        C3D_FrameDrawOn(top);

		shader.use();
		C3D_SetTexEnv(0, &env);
        skin.use();
        const C3D_Mtx skinMtx = skinTransform.toMtx();
        shader.setUniform4x4(GPU_VERTEX_SHADER, "projection", &projection);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "view", &lookAt);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "modelView", &skinMtx);
        skin.render();

        C3D_Mtx ViewProjectionMatrix;
        Mtx_Multiply(&ViewProjectionMatrix, &projection, &lookAt);

        C3D_FVec cameraRight = FVec3_New(lookAt.r[0].c[0], lookAt.r[0].c[1], lookAt.r[0].c[2]);
        C3D_FVec cameraUp  = FVec3_New(lookAt.r[1].c[0], lookAt.r[1].c[1], lookAt.r[1].c[2]);


        C3D_FVec billboardPos = FVec3_New(0, 0.f, -1.5f);
        // billboard.use();

        // billboard.setUniform(GPU_VERTEX_SHADER, "cameraRight", cameraRight.x, cameraRight.y, cameraRight.z, 1.f);
        // billboard.setUniform(GPU_VERTEX_SHADER, "cameraUp", cameraUp.x, cameraUp.y, cameraUp.z, 1.f);
        // billboard.setUniform(GPU_VERTEX_SHADER, "pos", billboardPos.x, billboardPos.y, billboardPos.z, billboardPos.w);
        
        // billboard.setUniform4x4(GPU_VERTEX_SHADER, "projection", &projection);
        // billboard.setUniform4x4(GPU_VERTEX_SHADER, "view", &lookAt);
        C3D_TexBind(0, &steve);
        C3D_SetAttrInfo(&attr);
        C3D_SetBufInfo(&bufInfo);
        C3D_DrawElements(GPU_TRIANGLES, 6, C3D_UNSIGNED_BYTE, eboBuffer);

        C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, C2D_Color32(1, 199, 199, 255), 0);
		C3D_FrameDrawOn(bottom);
		C2D_SceneTarget(bottom);
		C2D_Prepare();

		ui->draw(skin);
		C2D_Flush();

		C3D_FrameEnd(0);
    }
    C3D_TexDelete(&steve);
    linearFree(eboBuffer);
    linearFree(vbo);
    skin.cleanup();
    socExit();
    free(soc_sharedmem);
    soc_sharedmem = nullptr;
    C2D_Fini();
	C3D_Fini();
    gfxExit();
	romfsExit();
    return 0;
}
