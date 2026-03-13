#include <3ds.h>
#include <citro3d.h>

#include "utils.hpp"
#include "camera.hpp"

void Camera::update() {
    const u32 kDown = hidKeysDown();
    const u32 kHeld = hidKeysHeld();
	hidTouchRead(&touch);
    hidCircleRead(&cPad);
    hidCstickRead(&cStick);

    if(kDown & KEY_TOUCH)
        touchOld = touch;

    if(kHeld & KEY_TOUCH) {
        if(firstLook) {
            touchOld = touch;
            firstLook = false;
        }
        touchDelta = v2f(touch.px - touchOld.px, touchOld.py - touch.py);
        if(!viewLock)
            rotateCamera(touchDelta);
        
        touchOld = touch;
    }

    const C3D_FVec baseUp = FVec3_New(0, 1, 0);
    if (kHeld & KEY_L && kHeld & KEY_R) {
        roll = 0.f;
        up = FVec3_New(0, 1, 0);
    }
    else if(kHeld & KEY_R) {
        roll -= 5.f;
        C3D_Mtx rollMtx;
        Mtx_Identity(&rollMtx);
        Mtx_Rotate(&rollMtx, FVec3_Normalize(direction), C3D_AngleFromDegrees(roll), false);
        up = Mtx_MultiplyFVec3(&rollMtx, baseUp);
        // up = FVec3_Normalize(up);
    }
    else if(kHeld & KEY_L) {
        roll += 5.f;
        C3D_Mtx rollMtx;
        Mtx_Identity(&rollMtx);
        Mtx_Rotate(&rollMtx, FVec3_Normalize(direction), C3D_AngleFromDegrees(roll), false);
        up = Mtx_MultiplyFVec3(&rollMtx, baseUp);
        // up = FVec3_Normalize(up);
    }

	if(cPad.dy > cPadDeadzone || cPad.dy < -cPadDeadzone) {
		position = FVec3_Add(position, FVec3_Scale(front, cameraSpeed * (float)((float)cPad.dy/154.f)));
	}
	if((cPad.dx > cPadDeadzone || cPad.dx < -cPadDeadzone )&& !xLock) {
		position = FVec3_Add(position, FVec3_Scale(FVec3_Normalize(FVec3_Cross(front, up)), cameraSpeed * (float)((float)cPad.dx/154.f)));
	}
    if(!viewLock)
        rotateCamera(v2f(cStick.dx * 0.02, cStick.dy * 0.02));
}

void Camera::rotateCamera(v2f delta) {
    yaw += delta.x;
    pitch += delta.y;

    if(pitch > 89.f)
        pitch = 89.f;
    if(pitch < -89.f)
        pitch = -89.f;

    direction.x = cos(C3D_AngleFromDegrees(yaw)) * cos(C3D_AngleFromDegrees(pitch));
    direction.y = sin(C3D_AngleFromDegrees(pitch));
    direction.z = sin(C3D_AngleFromDegrees(yaw)) * cos(C3D_AngleFromDegrees(pitch));

    front = FVec3_Normalize(direction);
}

void Camera::resetAngle() {
    direction = FVec3_New(0, 0, 0);
    yaw = -90.f;
    pitch = 0.f;
    roll = 0.f;
    firstLook = true;
}

C3D_Mtx Camera::getLookAt() {
    C3D_Mtx lookAt;
    Mtx_LookAt(&lookAt, position, FVec3_Add(position, front), up, false);
    return lookAt;
}
