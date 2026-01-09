#pragma once

struct v2f {
    v2f() : x(0), y(0) {}
    v2f(float p_x, float p_y) : x(p_x), y(p_y) {}
    float x, y;
};

struct v3f {
    v3f() : x(0), y(0), z(0) {}
    v3f(float p_x) : x(p_x), y(p_x), z(p_x) {}
    v3f(float p_x, float p_y, float p_z) : x(p_x), y(p_y), z(p_z) {}
    float x, y, z;
};

struct vertex { 
    v3f position;
    v2f texCoords;
};

struct Transform {
    Transform(v3f p_pos, v3f p_rot, v3f p_scale = {1.f,1.f,1.f}) : pos(p_pos), rotation(p_rot), scale(p_scale) {}
    v3f pos;
    v3f rotation;
    v3f scale;
    C3D_Mtx toMtx() {
        C3D_Mtx mtx;
        Mtx_Identity(&mtx);
        Mtx_Translate(&mtx, pos.x, pos.y, pos.z, true);
        Mtx_RotateX(&mtx, C3D_AngleFromDegrees(rotation.x), true);
        Mtx_RotateY(&mtx, C3D_AngleFromDegrees(rotation.y), true);
        Mtx_RotateZ(&mtx, C3D_AngleFromDegrees(rotation.z), true);
        Mtx_Scale(&mtx, scale.x, scale.y, scale.z);
        return mtx;
    }
};