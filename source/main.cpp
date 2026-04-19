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

#include "nameplate.hpp"
#include "colour_shbin.h"
#include "billboard_shbin.h"
#include "tex_shbin.h"

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

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
    skin.download("Bob_gangster");
    Transform skinTransform (v3f(0,-1.25, -1), v3f(0.f), v3f(1.f));

	circlePosition cPad;

    Nameplate nameplate(skin.name);

    Shader billboard((u32*)billboard_shbin, billboard_shbin_size);

    while (aptMainLoop())
    {
        hidScanInput();
		hidCircleRead(&cPad);

        const u32 kDown = hidKeysDown();
        if(kDown & KEY_START)
            break;

        camera.update();
		ui->update(skin, nameplate, skinTransform, camera);
        
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

        Transform billboard(v3f(0,1, -0.75), v3f(0.f), v3f(.25f));
        billboard.pos.x += nameplate.offset * billboard.scale.x;
        const C3D_Mtx billboardMtx = billboard.toMtx();
        shader.setUniform4x4(GPU_VERTEX_SHADER, "modelView", &billboardMtx);
        // billboard.use();

        // billboard.setUniform(GPU_VERTEX_SHADER, "cameraRight", cameraRight.x, cameraRight.y, cameraRight.z, 1.f);
        // billboard.setUniform(GPU_VERTEX_SHADER, "cameraUp", cameraUp.x, cameraUp.y, cameraUp.z, 1.f);
        // billboard.setUniform(GPU_VERTEX_SHADER, "pos", billboardPos.x, billboardPos.y, billboardPos.z, billboardPos.w);
        
        // billboard.setUniform4x4(GPU_VERTEX_SHADER, "projection", &projection);
        // billboard.setUniform4x4(GPU_VERTEX_SHADER, "view", &lookAt);
        C3D_DepthTest(true, GPU_ALWAYS, GPU_WRITE_ALL);
        nameplate.render();
        C3D_DepthTest(false, GPU_ALWAYS, GPU_WRITE_ALL);

        C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, C2D_Color32(1, 199, 199, 255), 0);
		C3D_FrameDrawOn(bottom);
		C2D_SceneTarget(bottom);
		C2D_Prepare();

		ui->draw(skin);
		C2D_Flush();

		C3D_FrameEnd(0);
    }
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
