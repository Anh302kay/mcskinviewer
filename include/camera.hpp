#pragma once
#include <3ds.h>
#include <citro3d.h>
#include "utils.hpp"

class Camera {
public:
    Camera() { Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(90.f), C3D_AspectRatioTop, 0.01f, 1000.f, false); }
    Camera(float fov) {Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(fov), C3D_AspectRatioTop, 0.01f, 1000.f, false); }
    C3D_Mtx projection;
    C3D_FVec position = FVec3_New(0.f, 0.f, 1.f);
    C3D_FVec front = FVec3_New(0, 0, -1);
    C3D_FVec up = FVec3_New(0, 1, 0);
    C3D_FVec right = FVec3_New(0, 1, 0);

	C3D_FVec direction = FVec3_New(0, 0, 0);
    float yaw = -90.f, pitch = 0.f, roll = 0.f;
    bool firstLook = true;

    float cameraSpeed = 0.05f;
	int cPadDeadzone = 10;
    bool viewLock = false;

    void update();
    void rotateCamera(v2f delta);
    void resetAngle();
    C3D_Mtx getLookAt();
    void changeFOV(float fov) { Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(fov), C3D_AspectRatioTop, 0.01f, 1000.f, false); }

private:
    circlePosition cPad;
    circlePosition cStick;
	touchPosition touchOld = {0,0};
	touchPosition touch;
};