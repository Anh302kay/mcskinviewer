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

#include "utils.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "objloader.hpp"
#include "mesh.hpp"
#include "skin.hpp"
#include "glbloader.hpp"
#include "ui.hpp"

#include "colour_shbin.h"
#include "tex_shbin.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

float vertices[] = {
    	// First face (PZ)
	// First triangle
	 -0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
	 +0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	// Second triangle
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	 -0.5f, +0.5f, +0.5f, 0.0f, 1.0f,
	 -0.5f, -0.5f, +0.5f, 0.0f, 0.0f,

	// Second face (MZ)
	// First triangle
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	 -0.5f, +0.5f, -0.5f, 1.0f, 0.0f,
	 +0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
	// Second triangle
	 +0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
	 +0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

	// Third face (PX)
	// First triangle
	 +0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	 +0.5f, +0.5f, -0.5f, 1.0f, 0.0f,
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	// Second triangle
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	 +0.5f, -0.5f, +0.5f, 0.0f, 1.0f,
	 +0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

	// Fourth face (MX)
	// First triangle
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	 -0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
	 -0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	// Second triangle
	 -0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	 -0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

	// Fifth face (PY)
	// First triangle
	 -0.5f, +0.5f, -0.5f, 0.0f, 0.0f,
	 -0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	// Second triangle
	 +0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
	 +0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
	 -0.5f, +0.5f, -0.5f, 0.0f, 0.0f,

	// Sixth face (MY)
	// First triangle
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
	 +0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
	 +0.5f, -0.5f, +0.5f, 1.0f, 1.0f,
	// Second triangle
	 +0.5f, -0.5f, +0.5f, 1.0f, 1.0f,
	 -0.5f, -0.5f, +0.5f, 0.0f, 1.0f,
	 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
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

    C3D_AttrInfo* attribute = C3D_GetAttrInfo();
    AttrInfo_Init(attribute);
    AttrInfo_AddLoader(attribute, 0, GPU_FLOAT, 3); // pos
    AttrInfo_AddLoader(attribute, 1, GPU_FLOAT, 2); // uv

    void* vboData = linearAlloc(sizeof(vertices));
    memcpy(vboData, vertices, sizeof(vertices));

    C3D_BufInfo bufInfo;
    BufInfo_Init(&bufInfo);
    BufInfo_Add(&bufInfo, vboData, sizeof(vertex), 2, 0x10);

    C3D_Tex steve;
    loadTex("romfs:/gfx/steve.t3x", &steve, nullptr);
    C3D_TexSetFilter(&steve, GPU_NEAREST, GPU_NEAREST);
	C3D_TexSetWrap(&steve, GPU_CLAMP_TO_EDGE, GPU_CLAMP_TO_EDGE);
	C3D_TexBind(0, &steve);

    C3D_TexEnv env;
	C3D_TexEnvInit(&env);
	C3D_TexEnvSrc(&env, C3D_Both, GPU_TEXTURE0);
	C3D_TexEnvFunc(&env, C3D_Both, GPU_REPLACE);
	C3D_SetTexEnv(0, &env);

    Shader shader((u32*)tex_shbin, tex_shbin_size);
    shader.use();

    Camera camera;

    Transform transform(v3f(0,0,-2), v3f(0, 45, 45));
    Transform testTransform (v3f(0,0,-4), v3f(0.f, 0.f, 0.f), v3f(1.f));

    Skin skinStr;
    skinStr.initModels();
    skinStr.download("Bob_Gangster");

	button test(v2f(5), v2f(50), C2D_Color32(255,255,255,255), 3);
	touchPosition touch;
	
	C2D_SpriteSheet sheet = C2D_SpriteSheetLoad("romfs:/gfx/keyboard.t3x");
	C2D_Image keyboard = C2D_SpriteSheetGetImage(sheet, 0);

    while (aptMainLoop())
    {
        hidScanInput();
		hidTouchRead(&touch);
        
        const u32 kDown = hidKeysDown();
        if(kDown & KEY_START)
            break;

        camera.update();

        const C3D_Mtx lookAt = camera.getLookAt();
        const C3D_Mtx projection = camera.projection;

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C3D_RenderTargetClear(top, C3D_CLEAR_ALL, C2D_Color32(1, 128, 199, 255), 0);
        C3D_FrameDrawOn(top);

		shader.use();
		C3D_SetTexEnv(0, &env);

        const C3D_Mtx transformMTX = transform.toMtx();
	    C3D_TexBind(0, &steve);
        C3D_SetBufInfo(&bufInfo);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "projection", &projection);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "view", &lookAt);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "modelView", &transformMTX);
        C3D_DrawArrays(GPU_TRIANGLES, 0, sizeof(vertices)/(sizeof(vertex)));

        skinStr.use();
        const C3D_Mtx testTransformMTX = testTransform.toMtx();
        shader.setUniform4x4(GPU_VERTEX_SHADER, "projection", &projection);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "view", &lookAt);
        shader.setUniform4x4(GPU_VERTEX_SHADER, "modelView", &testTransformMTX);
        skinStr.render();

        C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, C2D_Color32(1, 199, 199, 255), 0);
		C3D_FrameDrawOn(bottom);
		C2D_SceneTarget(bottom);
		C2D_Prepare();
		
		C2D_DrawImageAt(keyboard, 0, 0, 0);
		test.draw();
		C2D_Flush();
        
		C3D_FrameEnd(0);
    }

	C2D_SpriteSheetFree(sheet);

    C3D_TexDelete(&steve);
    linearFree(vboData);

    socExit();
    free(soc_sharedmem);
    C2D_Fini();
	C3D_Fini();
    gfxExit();
	romfsExit();
    return 0;
}